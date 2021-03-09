#include <stdio.h>
#include <stdlib.h>

#include "huffman.h"


int main(int argc, char *argv[]){

    // check argument count
    if (argc != 7) return EXIT_FAILURE;

    // check for failure
    int failure;

    /* argv[2] */

    // reconstruct the coding tree
    Tree* codingTree = reconstructTree(argv[1]);
    if(codingTree == NULL) return EXIT_FAILURE;
    displayTreePreOrder(codingTree);

    // save the tree in character-based representation
    failure = saveTreeCharacterBased(codingTree, argv[2]);
    if(failure) return EXIT_FAILURE;

    /* argv[3] */

    // decode the input file and save it in argv[3]
    // count the frequency of ASCII and save it in array
    long charFrequency[256] = {0};
    failure = decodeFile(argv[1], codingTree, argv[3], charFrequency);
    if(failure) return EXIT_FAILURE;

    /* argv[4] */

    // save the ASCII frequencies in argv[4]
    failure = saveCharFreq(charFrequency, argv[4]);
    if(failure) return EXIT_FAILURE;

    /* argv[5] */

    // construct huffman coding tree using character frequency
    Tree* huffmanCodingTree = constructHuffmanTree(charFrequency);
    if(huffmanCodingTree == NULL) return EXIT_FAILURE;

    // and save it in argv[5]
    failure = saveTreeCharacterBased(huffmanCodingTree, argv[5]);
    if(failure) return EXIT_FAILURE;

    /* argv[6] */

    // save the bits required to encode the original file
    // for reconstructed tree and constructed huffman tree
    failure = saveBitsRequired(argv[3], codingTree, huffmanCodingTree, argv[6]);
    if(failure) return EXIT_FAILURE;

    // free up memory
    deleteTree(codingTree);
    deleteTree(huffmanCodingTree);

    return EXIT_SUCCESS;
}