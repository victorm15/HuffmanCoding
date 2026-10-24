#include <stdint.h>
#include <stdio.h>

#ifndef TYPES_H
#define TYPES_H

typedef struct {
    uint64_t buffer;
    int bit_count;

} Codeword;

typedef struct node {
    unsigned char symbol;
    uint64_t frequency;
    int left_child;
    int right_child;
} Node;

typedef struct {
    uint64_t n_symbols;
    int n_distinct_symbols;
} SymbolCounts;



#endif