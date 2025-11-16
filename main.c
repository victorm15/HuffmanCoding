#include <xlsxwriter.h>
#include <stdio.h>
#include <stdlib.h>
#include "huffman_tree.h"
#include "bit_writer.h"
#include <stdbool.h>
#include <math.h>


void format_sheet(lxw_worksheet *worksheet) {
    worksheet_write_string(worksheet,0,0,"Original Size (bits)",NULL);
    worksheet_write_string(worksheet,0,1,"Compressed Size (bits)",NULL);
    worksheet_write_string(worksheet,0,2,"Header Size (bits)",NULL);
    worksheet_write_string(worksheet,0,3,"Entropy (bits)",NULL);
    worksheet_write_string(worksheet,0,4,"Average Compressed Element Size (bits)",NULL);
    worksheet_write_string(worksheet,0,5,"Coding Efficiency (%%)",NULL);
    worksheet_write_string(worksheet,0,6,"Average Compressed Element Size with Header (bits)",NULL);
    worksheet_write_string(worksheet,0,7,"Coding Efficiency with Header (%%)",NULL);
    worksheet_write_string(worksheet,0,8,"Compression Ratio",NULL);
}


void compress(FILE *input, lxw_worksheet *worksheet) {

    int alphabet_frequencies[256] = {}; 

    int num_distinct_symbols = 0;

    uint64_t num_symbols = 0;

    // Build min-heap from input String
    scan_input(alphabet_frequencies, &num_distinct_symbols, input, &num_symbols);
    Node** symbols = heapify(&num_distinct_symbols, alphabet_frequencies);

    
    

    worksheet_write_number(worksheet, 1, 0, 8 * num_symbols, NULL);

    worksheet_write_number(worksheet, 1, 2, 10 * num_distinct_symbols + 7, NULL);

    // Build Huffman tree
    Node* tree = huffman_tree(symbols, &num_distinct_symbols);


    // Build lookup table
    Lookup table[256] = {};
    build_table(table, tree, 0, 0);

    // Find entropy and average codeword length
    double entropy = 0;
    double average_size = 0;
    for(int i = 0; i < 256; i ++) {
        if (alphabet_frequencies[i] > 0) {
            double probability = alphabet_frequencies[i] / (double) num_symbols;
            entropy -= probability * (log10f(probability) / log10f(2));
            average_size += probability * table[i].bit_count;
        }
        
    }

    worksheet_write_number(worksheet, 1, 3, entropy, NULL);
    worksheet_write_number(worksheet, 1, 4, average_size, NULL);
    worksheet_write_number(worksheet, 1, 5, entropy / average_size, NULL);
    double average_size_with_header = average_size + (10 * num_distinct_symbols + 7) / (double)num_symbols;
    worksheet_write_number(worksheet, 1, 6, average_size_with_header, NULL);
    worksheet_write_number(worksheet, 1, 7, entropy / average_size_with_header, NULL);




    // Bit Writer
    bit_editor *bw = malloc(sizeof(bit_editor));
    bw->bit_count = 0;
    bw->buffer = 0;
    bw->file = fopen("output.bin", "wb");

    if (bw->file == NULL) {
        perror("Unable to open output");
        return;
    }

    // Write Compressed File
    write_header(bw, &num_symbols);
    write_tree(bw, tree);
    write_input(bw, table, input);
    write_padding(bw);
    fclose(bw->file);


    FILE* output = fopen("output.bin", "rb");
    if (output == NULL) {
        perror("Unable to open output to check size\n");
        return;
    }
    fseek(output, 0L, SEEK_END);
    uint64_t compression_size = 8 * ftell(output);
    fclose(output);

    worksheet_write_number(worksheet, 1, 1, compression_size, NULL);
    worksheet_write_number(worksheet, 1, 8, (8 * num_symbols) / (double)compression_size, NULL);






}

char* decompress() {

    // Bit Reader
    bit_editor *br = malloc(sizeof(bit_editor));
    br->bit_count = 0;
    br->buffer = 0;
    br->file = fopen("output.bin", "rb");

    if (br->file == NULL) {
        perror("Unable to open output\n");
        return "";
    }

    // Header
    uint64_t read_num_symbols = 0;
    read_header(br, &read_num_symbols);

    // Tree
    Node* tree1 = malloc(sizeof(Node));
    read_tree(br, tree1);

    // Coded Text
    char* decoded_string = read_huffman_code(br, tree1, &read_num_symbols);

    fclose(br->file);
    

    return decoded_string;


}

void compress_by_block(FILE *input, lxw_worksheet *worksheet, uint64_t max_bytes) {

    
    for (uint64_t i = 1; true; i ++) {
        
        int alphabet_frequencies[256] = {};
        int num_distinct_symbols = 0;
        uint64_t num_symbols = 0;

        if (!scan_some_input(alphabet_frequencies, &num_distinct_symbols, input, &num_symbols, &max_bytes)) return;

        printf("Block: %llu\n", i);

        // Build min-heap from input String
        Node** symbols = heapify(&num_distinct_symbols, alphabet_frequencies);

        
        

        worksheet_write_number(worksheet, i, 0, 8 * num_symbols, NULL);

        worksheet_write_number(worksheet, i, 2, 10 * num_distinct_symbols + 7, NULL);

        // Build Huffman tree
        Node* tree = huffman_tree(symbols, &num_distinct_symbols);


        // Build lookup table
        Lookup table[256] = {};
        build_table(table, tree, 0, 0);

        // Find entropy and average codeword length
        double entropy = 0;
        double average_size = 0;
        for(int r = 0; r < 256; r ++) {
            if (alphabet_frequencies[r] > 0) {
                double probability = alphabet_frequencies[r] / (double) num_symbols;
                entropy -= probability * (log10f(probability) / log10f(2));
                average_size += probability * table[r].bit_count;
            }
            
        }

        worksheet_write_number(worksheet, i, 3, entropy, NULL);
        worksheet_write_number(worksheet, i, 4, average_size, NULL);
        worksheet_write_number(worksheet, i, 5, entropy / average_size, NULL);
        
        double average_size_with_header = average_size + (10 * num_distinct_symbols + 7) / (double)num_symbols;
        worksheet_write_number(worksheet, i, 6, average_size_with_header, NULL);
        worksheet_write_number(worksheet, i, 7, entropy / average_size_with_header, NULL);




        // Bit Writer
        bit_editor *bw = malloc(sizeof(bit_editor));
        bw->bit_count = 0;
        bw->buffer = 0;
        bw->file = fopen("output.bin", "wb");

        if (bw->file == NULL) {
            perror("Unable to open output");
            return;
        }

        // Write Compressed File
        write_header(bw, &num_symbols);
        write_tree(bw, tree);
        write_block(bw, table, input, &max_bytes);
        write_padding(bw);
        fclose(bw->file);


        FILE* output = fopen("output.bin", "rb");
        if (output == NULL) {
            perror("Unable to open output to check size\n");
            return;
        }
        fseek(output, 0L, SEEK_END);
        uint64_t compression_size = 8 * ftell(output);
        fclose(output);

        worksheet_write_number(worksheet, i, 1, compression_size, NULL);
        worksheet_write_number(worksheet, i, 8, (8 * num_symbols) / (double)compression_size, NULL);
        
    }



     

    

    

    


}




int main() {



    FILE *input_file = fopen("input.txt", "rb");
    if (input_file == NULL) {
        perror("Unable to open input\n");
        return -1;
    }

    lxw_workbook  *workbook  = workbook_new("results.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
    if (workbook == NULL) {
        perror("Unable to open workbook");
        return -1;
    }
    if (worksheet == NULL) {
        perror("Unable to open worksheet");
        return -1;
    }
    format_sheet(worksheet);
    
    
    // compress_by_block(input_file, worksheet, 100);

    compress(input_file, worksheet);

    


    fclose(input_file);
    workbook_close(workbook);

    printf("Compression complete\n");

    
    

   


    return 0;
}