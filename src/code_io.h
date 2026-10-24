#ifndef CODE_IO_H
#define CODE_IO_H

#include <stdint.h>
#include <stdio.h>
#include "types.h"
#include "huffman_tree.h"

#define BITS_PER_BYTE 8

typedef struct {
    unsigned char buffer;
    int bit_count;
    FILE* file;
} file_buffer;

void write_symbol_count(file_buffer *bw, uint64_t symbol_count);

void write_header(file_buffer *bw, const Node nodes[], int root_idx);

void write_contents(file_buffer *bw, const Codeword table[256], FILE *input);

void write_padding(file_buffer *bw);

uint64_t read_symbol_count(file_buffer *br);

void read_header(file_buffer *br, Node nodes[static 2*ALPHABET_SIZE-1], int* node_idx);

void write_decoded_contents(file_buffer *br, file_buffer *bw, const Node nodes[static 2*ALPHABET_SIZE-1], int root_idx, uint64_t symbol_count);

// void write_block(file_buffer *bw, Lookup table[256], FILE *input, uint64_t *max_bytes);

// void read_header(file_buffer *br, uint64_t *num_symbols);

// char* read_huffman_code(file_buffer *br, Node* tree, uint64_t *num_symbols);

// void read_tree(file_buffer *br, Node* tree);

#endif