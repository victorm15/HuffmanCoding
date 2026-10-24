#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <stdio.h>
#include <stdbool.h>
#include "types.h"

#define ALPHABET_SIZE 256

enum NodeType {Internal, Leaf};

// void print_huffman_tree(Node *tree, char *buffer, int length);

void build_leaves(Node nodes[], int node_heap[], const uint64_t symbol_frequencies[static ALPHABET_SIZE]);

void heapify(Node nodes[], int node_heap[], int distinct_symbols);

int build_huffman_tree(Node nodes[], int node_heap[], int n_nodes);

void build_lookup_table(Codeword symbol_to_codeword[static ALPHABET_SIZE], const Node nodes[], int root_idx, uint64_t buffer, int bit_count);

SymbolCounts scan_input(FILE *input, uint64_t symbol_frequencies[static ALPHABET_SIZE]);

static inline int is_leaf(const Node n) { return n.left_child == -1 && n.right_child == -1; }

// bool scan_some_input(int alphabet_frequencies[256], int *num_nodes, FILE *input, uint64_t *bytes_read, uint64_t *max_bytes);

// void free_tree(Node *tree);

#endif