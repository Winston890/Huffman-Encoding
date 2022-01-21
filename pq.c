#include "pq.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    uint32_t capacity;
    Node **nodes;
};

void insertion_sort(PriorityQueue *q) {
    for (uint32_t i = 0; i < pq_size(q) - 1; i++) {
        for (uint32_t j = i + 1; j < pq_size(q); j++) {
            if (q->nodes[i]->frequency > q->nodes[j]->frequency) {
                Node *temp = q->nodes[i];
                q->nodes[i] = q->nodes[j];
                q->nodes[j] = temp;
            }
        }
    }
}

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->head = 0;
        q->tail = 0;
        q->size = 0;
        q->capacity = capacity;
        q->nodes = (Node **) calloc(capacity, sizeof(Node *));
        if (!q->nodes) {
            free(q);
            q = NULL;
        }
    }
    assert(q);
    return q;
}

void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->nodes) {
        free((*q)->nodes);
        free(*q);
        *q = NULL;
    }
    return;
}

uint32_t pq_size(PriorityQueue *q) {
    if (q == NULL) {
        return 0;
    }
    return q->size;
}

bool pq_empty(PriorityQueue *q) {
    if (q->size == 0) {
        return true;
    }
    return false;
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = q->head; i != q->tail; i = (i + 1) % q->capacity) {
        node_print(q->nodes[i]);
    }
}

bool queue_full(PriorityQueue *q) {
    if (q->size == q->capacity) {
        return true;
    }
    return false;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (q->size == q->capacity) {
        return false;
    }
    bool inserted = false;
    for (uint32_t i = q->tail; i != q->head; i = ((i - 1 + q->capacity) % q->capacity)) {
        q->nodes[i] = q->nodes[(i - 1 + q->capacity) % q->capacity];
        if (n->frequency > q->nodes[(i - 1 + q->capacity) % q->capacity]->frequency) {
            q->nodes[(i - 1 + q->capacity) % q->capacity] = n;
            inserted = true;
            break;
        }
    }
    if (inserted == false) {
        q->nodes[q->head] = n;
    }
    q->size += 1;
    q->tail = (q->tail + 1) % q->capacity;
    insertion_sort(q);
    return true;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (q->size == 0) {
        return false;
    }
    q->size -= 1;
    *n = q->nodes[q->head];
    q->head = (q->head + 1) % q->capacity;
    return true;
}
