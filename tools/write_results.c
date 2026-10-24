#include "../src/types.h"
#include "../src/huffman_tree.h"
#include "../src/csv_writer.h"
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>

static bool analyse_input(const uint64_t symbol_frequencies[static ALPHABET_SIZE], const SymbolCounts counts, CompressionResult *result) {
    const int distinct_symbols = counts.n_distinct_symbols;
    if (distinct_symbols <= 1) return false;

    Node nodes[2*distinct_symbols-1];
    int node_heap[2*distinct_symbols-1];
    build_leaves(nodes, node_heap, symbol_frequencies);
    heapify(nodes, node_heap, distinct_symbols);
    const int tree_idx = build_huffman_tree(nodes, node_heap, distinct_symbols);

    *result = (CompressionResult){0};
    result->original_bits = counts.n_symbols * BITS_PER_BYTE;
    // 64-bit symbol count + tree (each leaf: 1 flag bit + 8 symbol bits, each internal node: 1 flag bit)
    result->header_bits = 64 + 10*distinct_symbols - 1;
    analyse_huffman_tree(nodes, tree_idx, counts.n_symbols, 0, result);
    result->compressed_bits += result->header_bits;
    result->compressed_bits += (BITS_PER_BYTE - result->compressed_bits % BITS_PER_BYTE) % BITS_PER_BYTE; // padding
    result->avg_adjusted_codeword_length = (double)result->compressed_bits / counts.n_symbols;
    return true;
}

int main(int argc, char** argv) {
    const char *in_path = argc > 1 ? argv[1] : "data/mobydick.txt";
    FILE *input = fopen(in_path, "rb");
    if (!input) {
        perror(in_path);
        return 1;
    }

    char *end = NULL;
    uint64_t chunk_size = argc > 2 ? strtoull(argv[2], &end, 10) : 0;
    if (argc > 2 && (*end != '\0' || chunk_size == 0 || argv[2][0] == '-')) {
        fprintf(stderr, "Invalid chunk size: %s\n", argv[2]); 
        return 1; 
    }
    if (mkdir("output", 0755) != 0 && errno != EEXIST) {
        perror("output");
        return 1;
    }
    const char *slash = strrchr(in_path, '/');
    const char *base = slash ? slash + 1 : in_path;
    const char *dot = strrchr(base, '.');
    int base_len = (dot && dot != base) ? (int)(dot - base) : (int)strlen(base);
    char csv_path[256];
    int path_len = chunk_size
        ? snprintf(csv_path, sizeof csv_path, "output/%.*s-%llu.csv", base_len, base, (unsigned long long)chunk_size)
        : snprintf(csv_path, sizeof csv_path, "output/%.*s.csv", base_len, base);
    if (path_len >= (int)sizeof csv_path) {
        fprintf(stderr, "Output path too long\n");
        return 1;
    }
    FILE* csv = fopen(csv_path, "w");
    if (!csv) {
        perror(csv_path); return 1;
    }
    write_csv_header(csv);

    CompressionResult result;
    if (chunk_size == 0) {
        uint64_t symbol_frequencies[ALPHABET_SIZE] = {0};
        const SymbolCounts counts = scan_input(input, symbol_frequencies);
        if (!analyse_input(symbol_frequencies, counts, &result)) {
            fprintf(stderr, "Input had no or only one distinct symbol read\n");
            return 1;
        }
        write_csv_entry(csv, &result);
    }
    else {
        uint64_t written = 0, skipped = 0;
        while (true) {
            uint64_t symbol_frequencies[ALPHABET_SIZE] = {0};
            const SymbolCounts counts = scan_input_chunk(input, symbol_frequencies, chunk_size);
            if (counts.n_symbols != chunk_size) break; // partial last chunk (or none left)
            if (analyse_input(symbol_frequencies, counts, &result)) {
                write_csv_entry(csv, &result);
                ++written;
            }
            else ++skipped;
        }
        if (skipped) {
            fprintf(stderr, "Skipped %llu chunk(s) with only one distinct symbol\n", (unsigned long long)skipped);
        }
        if (written == 0) {
            fprintf(stderr, "No full chunks of %llu bytes to write\n", (unsigned long long)chunk_size);
        }
    }

    fclose(input);
    if (fclose(csv) != 0) {
        perror(csv_path);
        return 1;
    }
    return 0;
}