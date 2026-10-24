#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <stdio.h>
#include "types.h"

void write_csv_header(FILE* csv);

void write_csv_entry(FILE* csv, const CompressionResult* entry);


#endif