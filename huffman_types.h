#include <stdint.h>

#ifndef HUFFMAN_TYPES_H

#define HUFFMAN_TYPES_H

typedef struct {
    uint32_t buffer;
    int bit_count;

} Lookup;

typedef struct node {

    char symbol;
    int frequency;

    struct node *left_child;
    struct node *right_child;


} Node;







#endif