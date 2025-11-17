# Huffman Coding

Constructing Huffman tree
Encoding tree
Encoding Characters
Decoding Compression

## Features
* Used a time-optimal min-heap priority queue to construct the Huffman tree
* Supports full-input compression or chunk-based compression based on byte count


## Constructing the Huffman Tree

1. Write symbol frequencies from an input
2. Built a min-heap priority queue based on symbol frequencies using Floyd's algorithm for linear-time heap construction
3. Merged nodes within the min-heap until a single node remained

## Writing Header

1. Write an 8-byte unsigned long of the number of symbols in input
2. Encode and write huffman tree
    a. Write a '0' bit for an internal node, or a '1' followed by the symbol held represented by ASCII for a leaf node
    b. Write left child
    c. Write right child
4. 
