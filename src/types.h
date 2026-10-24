#include <stdint.h>
#include <stdio.h>

#ifndef TYPES_H
#define TYPES_H

#define BITS_PER_BYTE 8
#define ALPHABET_SIZE 256

enum NodeType {Internal, Leaf};

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

typedef struct CompressionResult {
    uint64_t original_bits, compressed_bits, header_bits;
    double entropy, avg_codeword_length, avg_adjusted_codeword_length;
} CompressionResult;

typedef struct {
    unsigned char buffer;
    int bit_count;
    FILE* file;
} file_buffer;

#endif