#include "code.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

struct Code c;
//A stack of bytes/bitvectors, each operation works on a bit even though each item in stack is a byte
Code code_init(void) {
    c.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    if (c->top == 0) {
        return true;
    }
    return false;
}

bool code_full(Code *c) {
    if (c->top == MAX_CODE_SIZE) {
        return true;
    }
    return false;
}

bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    int offset = c->top % 8;
    c->bits[c->top / 8] |= bit << offset;
    c->top += 1;
    return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    int offset = c->top % 8;
    *bit = c->bits[c->top / 8] | (1 << offset);
    c->bits[c->top / 8] = c->bits[c->top / 8] & ~(1 << offset);
    c->top -= 1;
    return true;
}

void code_print(Code *c) {
    int bit = 0;
    int counter = 0;
    for (uint32_t i = 0; i < c->top; i++) {
        counter += 1;
        bit = (c->bits[i / 8] >> (i % 8)) & 1;
        printf("%d", bit);
        if ((counter % 8) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
