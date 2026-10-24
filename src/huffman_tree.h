#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <stdio.h>
#include <stdbool.h>
#include "types.h"

void build_leaves(Node nodes[], int node_heap[], const uint64_t symbol_frequencies[static ALPHABET_SIZE]);

void heapify(Node nodes[], int node_heap[], int distinct_symbols);

int build_huffman_tree(Node nodes[], int node_heap[], int n_nodes);

void build_lookup_table(Codeword symbol_to_codeword[static ALPHABET_SIZE], const Node nodes[], int root_idx, uint64_t buffer, int bit_count);

SymbolCounts scan_input(FILE *input, uint64_t symbol_frequencies[static ALPHABET_SIZE]);

SymbolCounts scan_input_chunk(FILE *input, uint64_t symbol_frequencies[static ALPHABET_SIZE], uint64_t max_symbols);

static inline int is_leaf(const Node n) { return n.left_child == -1 && n.right_child == -1; }

void analyse_huffman_tree(const Node nodes[], int node_idx, uint64_t n_symbols, int codeword_length, CompressionResult* result);

#endif