#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define OPTIONS "hi:o:"

void print_tree(Node *root, int outfile) {
    if (root->left != NULL && root->right != NULL) { //If internal node
        print_tree(root->left, outfile);
        print_tree(root->right, outfile);
        write_bytes(outfile, (uint8_t *) "I", 1);
    } else { //if leaf node
        write_bytes(outfile, (uint8_t *) "L", 1);
        write_bytes(outfile, (uint8_t *) &root->symbol, 1);
    }
}

struct stat statbuf;
typedef enum graph_types { h, i, o } graph_types;
int main(int argc, char **argv) {
    //Getopt stuff
    int opt = 0;
    bool help = false;
    int infile = STDIN_FILENO; //stdin, 0
    int outfile = STDOUT_FILENO; //stdout, 1
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        // Flag each program depending on what the program argument was
        case 'h': help = true; break;
        case 'i':
            close(infile);
            if ((infile = open(optarg, O_RDONLY)) == -1) {
                printf("error opening files");
                close(outfile);
                exit(1);
            }
            break;
        case 'o':
            close(outfile);
            if ((outfile = open(optarg, O_WRONLY | O_CREAT)) == -1) {
                printf("error opening filesdf");
                close(infile);
                exit(1);
            }
            break;
        default: help = true;
        }
    }

    if (help == true) {
        printf(
            "SYNOPSIS\n  A Hamming(8, 4) systematic code generator.\n\nUSAGE\n  ./encode [-h] [-i "
            "infile] [-o outfile]\n\nOPTIONS\n  -h             Program usage and help.\n  -i "
            "infile      Input data to encode.\n  -o outfile     Output of encoded data.\n");
        close(infile);
        close(outfile);
        return 0;
    }

    // Sets header fields
    fstat(infile, &statbuf);
    fchmod(outfile, statbuf.st_mode);
    Header header;
    header.magic = MAGIC;
    header.permissions = statbuf.st_mode;
    header.file_size = statbuf.st_size;
    uint64_t hist[ALPHABET];
    for (int i = 0; i < ALPHABET; i++) { // Clear out array
        hist[i] = 0;
    }
    hist[0] += 1; //To deal with empty file
    hist[255] += 1;
    uint8_t buffer[BLOCK] = { 0 };

    //Fill histogram, i is the symbol, hist[i] is freq of sym
    //Inspired a lil bit from Entropy.c, you can see my
    //implmentation that works below too, but it's a lot more convoluted
    int bytes_read = 0;
    while ((bytes_read = read(infile, buffer, BLOCK)) > 0) {
        for (int i = 0; i < bytes_read; i += 1) {
            hist[buffer[i]] += 1;
        }
    }
    // # unique symbols stored in count
    int count = 0;
    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i] != 0) {
            count += 1;
        }
    }
    header.tree_size = 3 * count - 1; //3* # of leaf nodes - 1;
    write_bytes(outfile, (uint8_t *) &header, sizeof(Header)); //Write the header
    //My implementation that is ugly compared to the entropy lol
    /*do {
        if (bytes_to_read >= BLOCK) {
            bytes_read = read_bytes(infile, buffer, BLOCK);
            bytes_to_read -= BLOCK;
        } else {
            bytes_read = read_bytes(infile, buffer, bytes_to_read);
            bytes_to_read = 0;
        }
        for (int byte = 0; byte < bytes_read; byte++) {
            hist[buffer[byte]] += 1;
        }
    } while (bytes_to_read > 0);
    }*/

    //Make the tree, print the tree dump, and make the codes
    Node *root = build_tree(hist);
    print_tree(root, outfile);
    Code table[ALPHABET];
    build_codes(root, table);
    lseek(infile, 0,
        SEEK_SET); //Because we already iterated through the file when making the histogram
    uint8_t nbuffer[BLOCK];
    uint32_t size = 0;
    while ((size = read_bytes(infile, nbuffer, BLOCK)) > 0) { //Write the codes
        for (uint32_t i = 0; i < size; i++) {
            write_code(outfile, &table[nbuffer[i]]);
        }
    }
    flush_codes(outfile); //Cleanup leftover codes and memory
    delete_tree(&root);
    close(outfile);
    close(infile);
    return 0;
}
