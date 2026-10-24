#include "code_io.h"
#include "types.h"
#include "huffman_tree.h"
#include <stdlib.h>

void write_bit(file_buffer *bw, char bit) {
    bw->buffer = (bw->buffer << 1) | (bit & 1);
    bw->bit_count ++;
    if (bw->bit_count == BITS_PER_BYTE) {
        if (fwrite(&bw->buffer, 1, 1, bw->file) != 1) { perror("write"); exit(1); }
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

void write_symbol_count(file_buffer *bw, uint64_t symbol_count) {
    for (int i = 63; i >= 0; --i) {
        write_bit(bw, (symbol_count >> i) & 1u);
    }
}

void write_tree(file_buffer *bw, const Node nodes[], int root_idx) {
    if (is_leaf(nodes[root_idx])) {
        write_bit(bw, Leaf);
        for(int i = BITS_PER_BYTE - 1; i >= 0; --i) {
            unsigned char bit = (nodes[root_idx].symbol >> i) & 1u;
            write_bit(bw, bit);
        }
    }
    else {
        write_bit(bw, Internal);
        write_tree(bw, nodes, nodes[root_idx].left_child);
        write_tree(bw, nodes, nodes[root_idx].right_child);
    }
}

void write_contents(file_buffer *bw, const Codeword symbol_to_codeword[256], FILE *input) {
    int c;
    while ((c = getc_unlocked(input)) != EOF) {
        for (int i = symbol_to_codeword[(unsigned char)c].bit_count - 1; i >= 0; i --) {
            int bit = (symbol_to_codeword[(unsigned char)c].buffer >> i) & 1u;
            write_bit(bw, bit);
        }
    }
}

void write_padding(file_buffer *bw) {
    if (bw->bit_count) {
        bw->buffer <<= (8 - bw->bit_count);
        if (fwrite(&(bw->buffer), 1, 1, bw->file) != 1) { perror("write"); exit(1); }
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

char read_bit(file_buffer *br) {
    if (br->bit_count == 0) {
        int c = fgetc(br->file);
        if (c == EOF) {
            if (ferror(br->file)) perror("read");
            else fprintf(stderr, "Input ended early: file is truncated or corrupt\n");
            exit(1);
        }
        br->buffer = (uint8_t)c;
        br->bit_count = 8;
    }
    char bit = (br->buffer >> 7) & 1u;
    br->buffer <<= 1;
    --br->bit_count;
    return bit;
}

uint64_t read_symbol_count(file_buffer *br) {
    uint64_t symbol_count = 0;
    for (int i = 0; i < 64; ++i) {
        symbol_count = (symbol_count << 1) | (read_bit(br) & 1u);
    }
    return symbol_count;
}

void read_header(file_buffer *br, Node nodes[static 2*ALPHABET_SIZE-1], int* node_idx) {
    char node_type = read_bit(br);
    if (node_type == Leaf) {
        unsigned char symbol = 0;
        for (int i = 0; i < BITS_PER_BYTE; ++i) {
            symbol = (symbol << 1) | (read_bit(br) & 1u);
        }
        nodes[*node_idx].symbol = symbol;
        nodes[*node_idx].left_child = -1;
        nodes[*node_idx].right_child = -1;
    }   
    else {
        int self = *node_idx;
        nodes[self].left_child = ++(*node_idx);
        read_header(br, nodes, node_idx);
        nodes[self].right_child = ++(*node_idx);
        read_header(br, nodes, node_idx);
    }
}

void write_symbol(file_buffer *br, file_buffer *bw, const Node nodes[static 2*ALPHABET_SIZE-1], int node_idx) {
    if (is_leaf(nodes[node_idx])) {
        if (fwrite(&(nodes[node_idx].symbol), 1, 1, bw->file) != 1) {
            perror("write"); exit(1);
        }
    }
    else {
        if (read_bit(br) == 0) {
            write_symbol(br, bw, nodes, nodes[node_idx].left_child);
        }
        else {
            write_symbol(br, bw, nodes, nodes[node_idx].right_child);
        }
    }
}

void write_decoded_contents(file_buffer *br, file_buffer *bw, const Node nodes[static 2*ALPHABET_SIZE-1], int root_idx, uint64_t symbol_count) {
    for (uint64_t i = 0; i < symbol_count; ++i) {
        write_symbol(br, bw, nodes, root_idx);
    }
}