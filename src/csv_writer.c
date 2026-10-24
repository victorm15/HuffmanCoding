#include "csv_writer.h"
#include <stdio.h>
#include <inttypes.h>

void write_csv_header(FILE* csv) {
    fprintf(csv, "Original Size (bits),Compressed Size (bits),Header Size (bits),Entropy (bits),"
        "Average Symbol Codeword length (bits),Average Symbol Codeword Length with Header (bits)\n");
}

void write_csv_entry(FILE* csv, const CompressionResult* entry) {
    fprintf(csv, "%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%f,%f,%f\n",
        entry->original_bits,
        entry->compressed_bits,
        entry->header_bits,
        entry->entropy,
        entry->avg_codeword_length,
        entry->avg_adjusted_codeword_length);
}