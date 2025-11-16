#include "bit_writer.h"

#include <stdbool.h>
#include <stdlib.h>



void write_bit(bit_editor *bw, int bit) {
    bw->buffer = (bw->buffer << 1) | (bit & 1);
    bw->bit_count ++;
    if (bw->bit_count == BITS_PER_BYTE) {
        fwrite(&(bw->buffer), 1, 1, bw->file);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

void write_header(bit_editor *bw, uint64_t *num_symbols) {
    fwrite(num_symbols, sizeof(*num_symbols), 1, bw->file);
}

void write_symbol(bit_editor *bw, Lookup table[256], unsigned char symbol) {

    for (int i = table[symbol].bit_count - 1; i >= 0; i --) {

        int bit = (table[symbol].buffer >> i) & 1u;
        write_bit(bw, bit);
    
    }



}

void write_input(bit_editor *bw, Lookup table[256], FILE *input) {
    rewind(input);
    
    while(true) {
        char symbol = fgetc(input);
        if (feof(input)) break;
        write_symbol(bw, table, (unsigned char)symbol);
    }
}

void write_block(bit_editor *bw, Lookup table[256], FILE *input, uint64_t *max_bytes) {
    fseek(input, -(long)(*max_bytes), SEEK_CUR);
    
    for(uint64_t i = 0; i < *max_bytes; i ++) {
        char symbol = fgetc(input);
        if (symbol == EOF) break;
        write_symbol(bw, table, (unsigned char)symbol);
    }

}

void write_padding(bit_editor *bw) {

    if (bw->bit_count) {
        bw->buffer <<= (8 - bw->bit_count);
        fwrite(&(bw->buffer), 1, 1, bw->file);
        bw->buffer = 0;
        bw->bit_count = 0;
    }


}

void write_tree(bit_editor *bw, Node* tree) {

    if (tree->symbol) {
        // Node is a leaf node

        write_bit(bw, 1); // Wrie '1' to signify leaf node

        // Write symbol
        for(int i = sizeof(tree->symbol) * BITS_PER_BYTE - 1; i >= 0; i --) {
            int bit = (tree->symbol >> i) & 1u;
            write_bit(bw, bit);
        }

    }
    else {
        // Node is an internal node

        // Write '0' to signify internal node
        write_bit(bw, 0);

        write_tree(bw, tree->left_child);
        write_tree(bw, tree->right_child);
    }

}





int read_bit(bit_editor *br) {

    // Read Byte
    if (br->bit_count == 0) {
        br->buffer = (uint8_t)fgetc(br->file);
        br->bit_count = 8;
    }

    // Get individiual bit (starting from MSB)
    int bit = (br->buffer >> 7) & 1;
    br->buffer <<= 1;
    br->bit_count --;

    return bit;
}

void read_header(bit_editor *br, uint64_t *num_symbols) {
    fread(num_symbols, sizeof(*num_symbols), 1, br->file);
}

void read_tree(bit_editor *br, Node* tree) {

    
    int is_leaf = read_bit(br);

    // If bit stores leaf node
    if (is_leaf) {
        char symbol = 0;

        for (int i = 0; i < 8; i++) {
            int bit = read_bit(br);
            symbol = (symbol << 1) | (bit & 1);

        }
        
        tree->symbol = symbol;
    }
    // If bit stores leaf node
    else {
        tree->left_child = malloc(sizeof(Node));
        read_tree(br, tree->left_child);

        tree->right_child = malloc(sizeof(Node));
        read_tree(br, tree->right_child);

    }


}

char read_symbol(bit_editor *br, Node* tree) {
    while (!tree->symbol) {
        int bit = read_bit(br);

        tree = bit? tree->right_child : tree->left_child;
    }
    return tree->symbol;

}

char* read_huffman_code(bit_editor *br, Node* tree, uint64_t *num_symbols) {    
    
    char *decoded_string = malloc((*num_symbols + 1) * sizeof(char));

    for (uint64_t symbols_read = 0; symbols_read < (*num_symbols); symbols_read ++) {
        char symbol = read_symbol(br, tree);
        decoded_string[symbols_read] = symbol;
    }

    decoded_string[*num_symbols] = '\0';
    
    return decoded_string;

}



