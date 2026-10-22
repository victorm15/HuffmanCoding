#include "huffman_tree.h"
#include <stdlib.h>
#include <string.h>


void Sink(Node **symbols, int index, int *num_nodes) {
    
    // Check if both children are accessible
    if(2 * index + 2 < *num_nodes) {
        int min_child = (symbols[2 * index + 1]->frequency < symbols[2 * index + 2]->frequency)? 2 * index + 1: 2 * index + 2;
        if (symbols[index]->frequency > symbols[min_child]->frequency) {
            Node* temp = symbols[index];
            symbols[index] = symbols[min_child];
            symbols[min_child] = temp;
            Sink(symbols, min_child, num_nodes);
        }
    }

    // If both are not accessible, check if left is
    else if (2 * index + 1 < *num_nodes) {

        if (symbols[index]->frequency > symbols[2 * index + 1]->frequency) {
            Node* temp = symbols[index];
            symbols[index] = symbols[2 * index + 1];
            symbols[2 * index + 1] = temp;
            Sink(symbols, 2 * index + 1, num_nodes);
        }
    }

}

Node* popMin(Node** symbols, int* num_nodes) {

    Node* popped = symbols[0];
    symbols[0] = symbols[*num_nodes - 1];

    Sink(symbols, 0, num_nodes);

    (*num_nodes) --;

    return popped;

}

void insert(Node** symbols, int* num_nodes, Node* new_node) {
    // No need to realloc — size of binary heap will never exceed original allocated size (2 removals for every insert)
    symbols[*num_nodes] = new_node;
    int parent = (*num_nodes - 1) / 2;
    (*num_nodes) ++;
    while (parent > 0) {
        Sink(symbols, parent, num_nodes);
        parent = (parent - 1) / 2;
    }
    Sink(symbols, 0, num_nodes);
}


void scan_input(int alphabet_frequencies[256], int *num_nodes, FILE *input, uint64_t *bytes_read) {

    // Set alphabet frequencies
    int c;
    while ((c = fgetc(input)) != EOF) {
        unsigned char symbol = (unsigned char)c;
        (*bytes_read) ++;
        if (!alphabet_frequencies[symbol]) (*num_nodes) ++;
        alphabet_frequencies[symbol]++;
    }
}

bool scan_some_input(int alphabet_frequencies[256], int *num_nodes, FILE *input, uint64_t *bytes_read, uint64_t *max_bytes) {
    
    int c;
    while (*bytes_read < *max_bytes && (c = fgetc(input)) != EOF) {
        unsigned char symbol = (unsigned char)c;
        if (alphabet_frequencies[symbol] == 0) (*num_nodes)++;
        alphabet_frequencies[symbol]++;
        (*bytes_read)++;
    }
    return *bytes_read > 0;
}



Node** heapify(int *num_nodes, const int alphabet_frequencies[256]) {

    



    // Put all symbols which appeared in an array of nodes with no children
    Node** symbols = malloc(*num_nodes * sizeof(Node*));

    int index = 0;
    for (int i = 0; i < 256; i ++) {
        if (alphabet_frequencies[i]) {
            Node* new_node = calloc(1, sizeof(Node));
            new_node->symbol = i;
            new_node->frequency = alphabet_frequencies[i];
            symbols[index] = new_node;
            index ++;
        }
    }


    // Construct min-heap using Floyd's algorithm
    for (int i = *num_nodes / 2 - 1; i >= 0; i--) {
        Sink(symbols, i, num_nodes);
    }

    
    
    
    return symbols;

}


Node* huffman_tree(Node** symbols, int *num_nodes) {

    while (*num_nodes != 1) {
        Node* n1 = popMin(symbols, num_nodes);
        Node* n2 = popMin(symbols, num_nodes);
        Node* new_node = calloc(1, sizeof(Node));
        new_node->frequency = n1->frequency + n2->frequency;
        new_node->left_child = n1;
        new_node->right_child = n2;
        insert(symbols, num_nodes, new_node);
    }

    return symbols[0];
}


void print_huffman_tree(Node *tree, char *buffer, int length) {

    if (is_leaf(tree)) {
        printf("%s: %c\n", buffer, tree->symbol);
    }
    else {
       length ++;
        char *new_buffer = malloc(length + 1);  // +1 for the null terminator
        memcpy(new_buffer, buffer, length - 1);
        new_buffer[length] = '\0';

        new_buffer[length-1] = '0';
        print_huffman_tree(tree->left_child, new_buffer, length);

        new_buffer[length-1] = '1';
        print_huffman_tree(tree->right_child, new_buffer, length);

        free(new_buffer);
    }


}

void build_table(Lookup table[256], Node* tree, uint64_t buffer, int length) {

    if (is_leaf(tree)) {
        table[tree->symbol].buffer = buffer;
        table[tree->symbol].bit_count = length;
        return;
    }

    build_table(table, tree->left_child, buffer << 1, length + 1);
    
    build_table(table, tree->right_child, buffer << 1 | 1, length + 1);

}

void free_tree(Node *tree) {
    if (tree == NULL) return;
    free_tree(tree->left_child);
    free_tree(tree->right_child);
    free(tree);
}