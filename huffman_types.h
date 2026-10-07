#include <stdint.h>

#ifndef HUFFMAN_TYPES_H

#define HUFFMAN_TYPES_H

typedef struct {
    uint64_t buffer;
    int bit_count;

} Lookup;

typedef struct node {

    unsigned char symbol;
    int frequency;

    struct node *left_child;
    struct node *right_child;


} Node;

static inline int is_leaf(const Node *n) {
    return n->left_child == NULL && n->right_child == NULL;
}







#endif