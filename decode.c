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

    //Check for correct file, and set perms
    Header header;
    read(infile, &header, sizeof(Header));
    if (header.magic != MAGIC) {
        printf("Not the right file.");
        exit(1);
    }
    fstat(infile, &statbuf);
    fchmod(outfile, header.permissions);

    //Bulk of decode, read in tree dump and recreate an identical tree
    uint8_t tree_dump[header.tree_size];
    read_bytes(infile, tree_dump, header.tree_size);
    Node *root = rebuild_tree(header.tree_size, tree_dump);
    assert(root);
    Node *copy_root = root;
    uint8_t byte = 0;
    uint64_t written_chars = 0;
    //Go through identical tree (creating codes by going left (0) and right (1))
    //And then write matching symbol to recreate file
    while ((read_bit(infile, &byte) == true) && written_chars < header.file_size) {
        if (byte == 0) {
            root = root->left;
        } else {
            root = root->right;
        }
        if (root->left == NULL && root->right == NULL) {
            written_chars += 1;
            write_bytes(outfile, &root->symbol, 1);
            root = copy_root;
        }
    }
    //Cleanup
    delete_tree(&copy_root);
    close(infile);
    close(outfile);
    return 0;
}
