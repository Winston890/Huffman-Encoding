#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n == NULL) {
        return NULL;
    }
    n->symbol = symbol;
    n->frequency = frequency;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void node_delete(Node **n) {
    free(*n);
    n = NULL;
}

//Joins two nodes, sym is always $
Node *node_join(Node *left, Node *right) {
    uint64_t joined_frequency = left->frequency + right->frequency;
    Node *n = node_create('$', joined_frequency);
    n->left = left;
    n->right = right;
    return n;
}

void node_print(Node *n) {
    if (n) {
        printf("Node Symbol: %d\tFrequency: "
               "%" PRIu64 "\n",
            n->symbol, n->frequency);
    }
}
