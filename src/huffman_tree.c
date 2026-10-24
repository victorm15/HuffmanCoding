#include "types.h"
#include "huffman_tree.h"

SymbolCounts scan_input(FILE *input, uint64_t symbol_frequencies[static ALPHABET_SIZE]) {
    int c;
    SymbolCounts counts = {.n_symbols = 0, .n_distinct_symbols = 0};
    while ((c = getc_unlocked(input)) != EOF) {
        unsigned char symbol = (unsigned char)c;
        if (symbol_frequencies[symbol] == 0) ++counts.n_distinct_symbols;
        ++symbol_frequencies[symbol];
        ++counts.n_symbols;
    }
    return counts;
}

void build_leaves(Node nodes[], int node_heap[], const uint64_t symbol_frequencies[static ALPHABET_SIZE]) {
    int node_idx = 0;
    for (int i = 0; i < ALPHABET_SIZE; i ++) {
        if (symbol_frequencies[i] > 0) {
            nodes[node_idx].frequency = symbol_frequencies[i];
            nodes[node_idx].symbol = (unsigned char)i;
            nodes[node_idx].left_child = -1;
            nodes[node_idx].right_child = -1;
            node_heap[node_idx] = node_idx;
            ++node_idx;
        }
    }
}

void Sink(Node nodes[], int node_heap[], int heap_size, int index) {
    while (true) {
        if (2*index + 1 >= heap_size) break;
        int min_child;
        min_child = (2*index+2 < heap_size && nodes[node_heap[2*index+1]].frequency > 
            nodes[node_heap[2*index+2]].frequency) ? 2*index+2 : 2*index+1;
        if (nodes[node_heap[min_child]].frequency >= nodes[node_heap[index]].frequency) break;
        int temp = node_heap[index];
        node_heap[index] = node_heap[min_child];
        node_heap[min_child] = temp;
        index = min_child;
    }
}

void heapify(Node nodes[], int node_heap[], int distinct_symbols) {
    for (int i = distinct_symbols / 2 - 1; i >= 0; --i) {
        Sink(nodes, node_heap, distinct_symbols, i);
    }
}

int popMin(Node nodes[], int node_heap[], int* heap_size) {
    int node = node_heap[0];
    node_heap[0] = node_heap[*heap_size-1];
    --(*heap_size);
    Sink(nodes, node_heap, *heap_size, 0);
    return node;
}

void insert(Node nodes[], int node_heap[], int* heap_size, int new_node) {
    int i = (*heap_size)++;
    node_heap[i] = new_node;
    while (i > 0) {
        int parent = (i-1)/2;
        if (nodes[node_heap[parent]].frequency <= nodes[node_heap[i]].frequency) break;
        int temp = node_heap[parent];
        node_heap[parent] = node_heap[i];
        node_heap[i] = temp;
        i = parent;
    }
}

int build_huffman_tree(Node nodes[], int node_heap[], int n_nodes) {
    int heap_size = n_nodes;
    while (heap_size != 1) {
        int node1 = popMin(nodes, node_heap, &heap_size);
        int node2 = popMin(nodes, node_heap, &heap_size);
        n_nodes++;
        nodes[n_nodes-1].frequency = nodes[node1].frequency + nodes[node2].frequency;
        nodes[n_nodes-1].left_child = node1;
        nodes[n_nodes-1].right_child = node2;
        insert(nodes, node_heap, &heap_size, n_nodes-1);
    }
    return n_nodes-1;
}

void build_lookup_table(Codeword symbol_to_codeword[static ALPHABET_SIZE], const Node nodes[], int root_idx, uint64_t buffer, int bit_count) {
    if (is_leaf(nodes[root_idx])) {
        symbol_to_codeword[nodes[root_idx].symbol].buffer = buffer;
        symbol_to_codeword[nodes[root_idx].symbol].bit_count = bit_count;
        return;
    }
    build_lookup_table(symbol_to_codeword, nodes, nodes[root_idx].left_child, buffer << 1, bit_count + 1);
    build_lookup_table(symbol_to_codeword, nodes, nodes[root_idx].right_child, buffer << 1 | 1u, bit_count + 1);
}