#include "../src/types.h"
#include "../src/code_io.h"
#include "../src/huffman_tree.h"
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    const char *in_path = argc > 1 ? argv[1] : "data/mobydick.txt";
    FILE *input = fopen(in_path, "rb");
    if (!input) {
        perror(in_path);
        return 1;
    }
    char out_path[256];
    if (argc > 2) {
        snprintf(out_path, sizeof out_path, "%s", argv[2]);
    } else {
        const char *slash = strrchr(in_path, '/');
        const char *base = slash ? slash + 1 : in_path;
        const char *dot = strrchr(base, '.');
        int base_len = (dot && dot != base) ? (int)(dot - base) : (int)strlen(base);
        if (mkdir("output", 0755) != 0 && errno != EEXIST) {
            perror("output");
            return 1;
        }
        if (snprintf(out_path, sizeof out_path, "output/%.*s.bin", base_len, base) >= (int)sizeof out_path) {
            fprintf(stderr, "Output path too long\n");
            return 1;
        }
    }
    FILE* output = fopen(out_path, "wb");
    if (!output) {
        perror(out_path); return 1;
    }

    uint64_t symbol_frequencies[256] = {0};
    const SymbolCounts counts = scan_input(input, symbol_frequencies);
    const int distinct_symbols = counts.n_distinct_symbols;
    if (distinct_symbols <= 1) {
        fprintf(stderr, "Input had no or only one distinct symbol read\n");
        return 1;
    }
    Node nodes[2*distinct_symbols-1];
    int node_heap[2*distinct_symbols-1];
    build_leaves(nodes, node_heap, symbol_frequencies);
    heapify(nodes, node_heap, distinct_symbols);
    const int tree_idx = build_huffman_tree(nodes, node_heap, distinct_symbols);
    Codeword symbol_to_codeword[ALPHABET_SIZE] = {0};
    build_lookup_table(symbol_to_codeword, nodes, tree_idx, 0, 0);

    rewind(input);
    file_buffer bw = {.buffer = 0, .bit_count = 0, .file = output};
    write_symbol_count(&bw, counts.n_symbols);
    write_header(&bw, nodes, tree_idx);
    write_contents(&bw, symbol_to_codeword, input);
    write_padding(&bw);

    fclose(input);
    fclose(output);

    return 0;
}
