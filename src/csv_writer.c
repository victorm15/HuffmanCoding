#include "csv_writer.h"
#include <stdio.h>

void write_csv_header(FILE* csv) {
    fprintf(csv, "Original Size (bits),Compressed Size (bits),Header Size (bits),Entropy (bits),"
        "Average Symbol Codeword length (bits),Coding Efficiency (%%),Average Symbol Codeword Length with Header (bits),"
        "Coding Efficiency with Header (bits),Compression Ratio");
}