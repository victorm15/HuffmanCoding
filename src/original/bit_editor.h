#include <stdint.h>
#include <stdio.h>
#include "huffman_types.h"

#ifndef BIT_EDITOR_H
#define BIT_EDITOR_H

#define BITS_PER_BYTE 8

typedef struct {
    unsigned char buffer;
    int bit_count;
    FILE* file;
} bit_editor;

void write_header(bit_editor *bw, uint64_t *num_symbols);

void write_tree(bit_editor *bw, Node* tree);

void write_input(bit_editor *bw, Lookup table[256], FILE *input);

void write_block(bit_editor *bw, Lookup table[256], FILE *input, uint64_t *max_bytes);

void write_padding(bit_editor *bw);

void read_header(bit_editor *br, uint64_t *num_symbols);

char* read_huffman_code(bit_editor *br, Node* tree, uint64_t *num_symbols);

void read_tree(bit_editor *br, Node* tree);

#endif