#include "io.h"

#include "code.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint8_t buffer[BLOCK];
static uint64_t bit_index = 0;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    bytes_read = 0;
    int instance_bytes_read = 0;
    while (nbytes > 0
           && (instance_bytes_read = read(infile, buf, nbytes))
                  > 0) { //Basically, while there are bytes to read and the file has not ended
        bytes_read += instance_bytes_read;
        nbytes -= instance_bytes_read;
        buf += instance_bytes_read; //Move buffer ptr so we don't reread
    }
    return bytes_read;
}

//Same as read_bytes
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    bytes_written = 0;
    int instance_bytes_written = 0;
    while (nbytes > 0 && (instance_bytes_written = write(outfile, buf, nbytes)) > 0) {
        bytes_written += instance_bytes_written;
        nbytes -= instance_bytes_written;
        buf += instance_bytes_written;
    }
    return bytes_written;
}

static uint64_t bytes_in = 0;
static uint64_t last_bit = 0;
bool read_bit(int infile, uint8_t *bit) {
    //Number of BYTES left to read
    //Complex logic
    //index = 0 means we need to read in more bytes
    if (bit_index == 0) {
        bytes_in = read_bytes(infile, buffer, BLOCK);
    }
    //if the bytes we read in (bytes_in) < BLOCK, that means
    //we reached the end of the file, and we set the last bit we need to read accordingly
    if (bytes_in < BLOCK) {
        last_bit = bytes_in * 8 + 1;
    }
    //We get the bit we're returning
    *bit = (buffer[bit_index / 8] >> (bit_index % 8)) & 1;
    //Increment bit index bc we just read a bit
    bit_index += 1;
    //if we read a full BLOCK then we gotta reset index so that we can read in more bytes
    if (bit_index == BLOCK * 8) {
        bit_index = 0;
    }
    //if we hit last bit to read, then terminate
    if (bit_index == last_bit) {
        bit_index = 0;
        return false;
    }
    return true;
}

void write_code(int outfile, Code *c) {
    for (uint8_t i = 0; i < code_size(c); i++) {
        //Gets bit so we can set/clear bit depending on if we get a 1/0
        int bit = (c->bits[i / 8] >> (i % 8)) & 1;
        int offset = bit_index % 8;
        if (bit == 1) {
            //sets bit in buffer;
            buffer[bit_index / 8] |= 1 << offset;
        } else {
            //clears bit in buffer;
            buffer[bit_index / 8] = buffer[bit_index / 8] & ~(1 << offset);
        }
        bit_index += 1;
        //Write if buffer full
        if (bit_index == BLOCK * 8) {
            write_bytes(outfile, buffer, BLOCK);
            bit_index = 0;
        }
    }
}

void flush_codes(int outfile) {
    if (bit_index != 0) {
        write_bytes(outfile, buffer, (bit_index / 8) + 1);
    }
    bit_index = 0;
}
