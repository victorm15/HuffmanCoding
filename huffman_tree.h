#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "huffman_types.h"

#ifndef HUFFMAN_TREE_H

#define HUFFMAN_TREE_H





Node** heapify(int *num_nodes, const int alphabet_frequencies[256]);

Node* huffman_tree(Node** symbols, int *num_nodes);

void print_huffman_tree(Node *tree, char *buffer, int length);

void build_table(Lookup table[256], Node* tree, uint32_t buffer, int length);

void scan_input(int alphabet_frequencies[256], int *num_nodes, FILE *input, uint64_t *bytes_read);

bool scan_some_input(int alphabet_frequencies[256], int *num_nodes, FILE *input, uint64_t *bytes_read, uint64_t *max_bytes);



#endif