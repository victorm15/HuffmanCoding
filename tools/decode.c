#include "../src/types.h"
#include "../src/code_io.h"
#include "../src/huffman_tree.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    const char *in_path = argc > 1 ? argv[1] : "output/mobydick.bin";
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
        int base_len = (int)strlen(base);
        if (base_len > 4 && strcmp(base + base_len - 4, ".bin") == 0) base_len -= 4;
        if (mkdir("output", 0755) != 0 && errno != EEXIST) {
            perror("output");
            return 1;
        }
        if (snprintf(out_path, sizeof out_path, "output/%.*s.txt", base_len, base) >= (int)sizeof out_path) {
            fprintf(stderr, "Output path too long\n");
            return 1;
        }
    }
    FILE* output = fopen(out_path, "wb");
    if (!output) {
        perror(out_path); return 1;
    }

    file_buffer br = {.buffer = 0, .bit_count = 0, .file = input};
    file_buffer bw = {.buffer = 0, .bit_count = 0, .file = output};

    Node nodes[2*ALPHABET_SIZE-1]; // upper-bound on tree size
    const uint64_t symbol_count = read_symbol_count(&br);
    // An empty original file is stored as just the zero symbol count, with no tree
    if (symbol_count > 0) {
        int tree_idx = 0;
        read_header(&br, nodes, &tree_idx);
        tree_idx = 0;
        write_decoded_contents(&br, &bw, nodes, tree_idx, symbol_count);
    }

    fclose(input);
    if (fclose(output) != 0) {
        perror(out_path);
        return 1;
    }


    return 0;
}