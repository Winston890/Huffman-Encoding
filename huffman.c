#include "huffman.h"

#include "code.h"
#include "defines.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);
    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i] != 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(pq, n);
        }
    }
    while (pq_size(pq) >= 2) {
        Node *left;
        Node *right;
        dequeue(pq, &left);
        dequeue(pq, &right);
        Node *parent = node_join(left, right);
        enqueue(pq, parent);
    }
    Node *root;
    dequeue(pq, &root);
    pq_delete(&pq);
    return root;
}

static uint8_t discard = 0;
void build(Node *root, Code table[static ALPHABET], Code c) {
    if (root->left == NULL && root->right == NULL) {
        table[root->symbol] = c;
    }
    if (root->left != NULL) {
        code_push_bit(&c, 0);
        build(root->left, table, c);
        code_pop_bit(&c, &discard);
    }
    if (root->right != NULL) {
        code_push_bit(&c, 1);
        build(root->right, table, c);
        code_pop_bit(&c, &discard);
    }
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    Code code = code_init();
    build(root, table, code);
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(MAX_TREE_SIZE);
    for (uint8_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], 0);
            stack_push(s, n);
            i += 1;
        } else {
            Node *left;
            Node *right;
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    Node *root;
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

void delete_tree(Node **root) {
    if ((*root)->left != NULL && (*root)->right != NULL) { //If internal node
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);
        node_delete(root);
    } else { //if leaf node
        node_delete(root);
    }
}
