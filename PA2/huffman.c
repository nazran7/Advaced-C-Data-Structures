#include <stdio.h>
#include <stdlib.h>

#include "huffman.h"


/* helper functions */

Tree* createNewTree(char val){
    Tree* newTree = (Tree*)malloc(sizeof(Tree));
    newTree->value = val;
    newTree->left = NULL;
    newTree->right = NULL;
    newTree->weight = 0;
    return newTree;
}


TreeList* createNewTreeList(Tree* tree){
    TreeList* treeList;
    treeList = (TreeList*)malloc(sizeof(TreeList));

    treeList->tree = tree;
    treeList->next = NULL;

    return treeList;
}


void deleteTreeList(TreeList* treeList){
    if(treeList == NULL) return;

    TreeList* temp = treeList->next;
    free(treeList);

    deleteTreeList(temp);
}


void deleteTree(Tree* tree){
    if(tree == NULL) return;
    // recursively free up left & right nodes
    Tree* left = tree->left;
    Tree* right = tree->right;
    free(tree);
    if(left != NULL) deleteTree(left);
    if(right != NULL) deleteTree(right);
}


int isLeafNode(Tree* tree){
    // return 1, if leaf node
    // return 0, if not leaf node
    return tree->left == NULL && tree->right == NULL;
}


void displayTreePreOrderRecurse(Tree* tree){
    if(tree == NULL) return;
    printf("%c", tree->value);
    displayTreePreOrderRecurse(tree->left);
    displayTreePreOrderRecurse(tree->right);
}


void displayTreePreOrder(Tree* tree){
    if(tree == NULL){
        printf("\nThe tree is empty!");
        return;
    }
    printf("\nThe tree elements are:\n");
    displayTreePreOrderRecurse(tree);
    printf("\n\n");
}


int getBitByIndex(char byte, int index){
    // create mask for index'th bit
    char mask = 1 << index;
    // apply mask and extract bit value
    int bit = (int)(((byte & mask) >> index) & 1);

    return bit;
}


Tree* reconstructTreeRecurse(FILE* f, int* index){
    // create a new tree
    Tree* newTree = createNewTree('\0');
    // read a byte from file
    char byte; 
    //printf("index value is........%d\n", *index);
    if(fread(&byte, 1, 1, f) != 1) {
        printf("byte is not read from file.........%d\n", *index);
        return NULL;
    }
    int bit = getBitByIndex(byte, *index);
    //printf("the value of bit is %d\n", bit);
    // if bit is 0, pass to left & right, bcoz not a leaf
    if(bit == 0){
        // increase index
        *index = *index + 1;
        if(*index > 7) *index = 0;
        if(*index != 0){
            // reset the file stream position to previous byte
            fseek(f, -1, SEEK_CUR);
        }
        newTree->left = reconstructTreeRecurse(f, index);
        newTree->right = reconstructTreeRecurse(f, index);
    }
    // if bit is 1, assign character, stop, bcoz leaf.
    if(bit == 1){
        // increase index, so that the 1 bit is ignored
        *index = *index + 1;
        if(*index > 7) *index = 0;
        // form a ASCII symbol byte
        char symbol = 0;
        // if index is zero, make the next byte as symbol
        if(*index == 0){
            // read next byte
            if(fread(&byte, 1, 1, f) != 1) {
                printf("byte is not read from file.........%d\n", *index);
                return NULL;
            }
            // fill the symbol with next byte
            symbol = byte;
        }
        // if index is not zero, read based on index
        else{
            // fill its LSBs with remaining MSBs of current byte
            symbol = (byte >> *index) & (0xFF >> *index);
            // read next byte
            if(fread(&byte, 1, 1, f) != 1) {
                printf("byte is not read from file.........%d\n", *index);
                return NULL;
            }
            // reset file stream position to previous byte
            fseek(f, -1, SEEK_CUR);
            // fill its MSBs with remaining LSBs of next byte
            symbol = symbol | ((byte << (8 - (*index))) & (0xFF << (8 - (*index))));
        }
        // assign symbol to value
        newTree->value = symbol;
        //printf("%c\n", symbol);
    }

    // return the new tree
    return newTree;
}


void saveTreeCharacterBasedRecurse(Tree* tree, FILE* f){
    if(tree == NULL) return;

    // symbol to write
    char symbol;

    // do preorder traversal

    // process tree
    // if tree is a leaf, save '1' and tree value to file.
    if(isLeafNode(tree)){
        symbol = '1';
        fwrite(&symbol, 1, 1, f);
        symbol = tree->value;
        fwrite(&symbol, 1, 1, f);
    }
    // if tree is not a leaf, save '0' to file.
    else{
        symbol = '0';
        fwrite(&symbol, 1, 1, f);
    }

    // process left of tree
    saveTreeCharacterBasedRecurse(tree->left, f);

    // process right of tree
    saveTreeCharacterBasedRecurse(tree->right, f);

}


int compare(Tree* first, Tree* second){
    // return 0, if first > second
    // return 1, if first < second

    // weight comparison
    if(first->weight < second->weight) return 1;
    if(first->weight > second->weight) return 0;

    // leaf or not leaf comparison
    if(isLeafNode(first)){
        if(isLeafNode(second)){
            // both are leaf
            return first->value < second->value;
        }
        else{
            // first is leaf and second is not leaf
            return 1;
        }
    }
    else{
        if(isLeafNode(second)){
            // first is not leaf and second is leaf
            return 0;
        }
        else{
            // both are not leaf
            return 0;
        }
    }

    return 1;
}


void sortedInsert(TreeList** treeList, Tree* newTree){
    TreeList* newTreeList = createNewTreeList(newTree);
    // empty treelist
    if(*treeList == NULL){
        *treeList = newTreeList;
        return;
    }
    // head is greater than new
    if(compare(newTree, (*treeList)->tree)){
        // insert front
        newTreeList->next = *treeList;
        *treeList = newTreeList;
        return;
    }
    // find correct place and insert
    TreeList *prevTreeList = *treeList, *currTreeList = (*treeList)->next;
    while(currTreeList != NULL){
        // check if its the correct position
        if(compare(newTree, currTreeList->tree)){
            prevTreeList->next = newTreeList;
            newTreeList->next = currTreeList;
            return;
        }

        prevTreeList = currTreeList;
        currTreeList = currTreeList->next;
    }
    // insert last
    prevTreeList->next = newTreeList;
    return;
}


TreeList* constructTreeList(long charFreq[]){
    // create new tree for characters of non-zero frequency
    // and store them as a priority queue using TreeList
    TreeList* treeList = NULL;
    Tree* new;
    for(int i = 0; i < 256; i++){
        if(charFreq[i] != 0){
            new = createNewTree((char)i);
            new->weight = charFreq[i];
            // inserted in sorted order
            sortedInsert(&treeList, new);
        }
    }
    return treeList;
}


void deleteFirstTwo(TreeList** treeList, Tree** first, Tree** second){
    // store first
    if(*treeList == NULL) return;
    TreeList* firstTreeList = *treeList;
    *first = firstTreeList->tree;

    // store second
    if(firstTreeList->next== NULL) return;
    TreeList* secondTreeList = firstTreeList->next;
    *second = secondTreeList->tree;

    // change head
    *treeList = secondTreeList->next;

    // delete the first two
    free(firstTreeList);
    free(secondTreeList);
}


Tree* combineTwoTrees(Tree* first, Tree* second){
    Tree* newTree = createNewTree('\0');
    // assign childs
    newTree->left = first;
    newTree->right = second;
    // assign sum of weights
    newTree->weight = first->weight + second->weight;

    return newTree;
}


void buildEncodingTable(Tree* tree, long numBitsEachSymbol[], long depth){
    // recursively count the depth and assign to array
    if(isLeafNode(tree)){
        numBitsEachSymbol[(int)tree->value] = depth;
        return;
    }
    buildEncodingTable(tree->left, numBitsEachSymbol, depth + 1);
    buildEncodingTable(tree->right, numBitsEachSymbol, depth + 1);
}


long calculateBitsRequired(char* filename, Tree* tree){
    long numBits = 0;
    // open input file for reading
    FILE* f = fopen(filename, "rb");
    if(f == NULL){
        printf("The input file cannot be opened '%s'", filename);
        return EXIT_FAILURE;
    }
    // bits required for each ASCII symbol
    long numBitsEachSymbol[256];
    buildEncodingTable(tree, numBitsEachSymbol, 0);
    // read through file and calculate total bits required
    char symbol;
    while(1){
        if(fread(&symbol, 1, 1, f) != 1) break;
        numBits += numBitsEachSymbol[(int)symbol];
    }

    fclose(f);
    return numBits;
}


void saveBits(long bits, FILE* f){

    // number of full bytes
    long int numBytes = bits / 8;
    fwrite(&numBytes, sizeof(long int), 1, f);

    // remaining bits
    int numBits = bits % 8;
    fwrite(&numBits, sizeof(int), 1, f);

    //printf("\nsave bits are:\n%ld\n%ld\n%d\n", bits, numBytes, numBits);
}


/* Main functions */

Tree* reconstructTree(char* filename){
    // open file
    FILE *f;
    f = fopen(filename, "rb");
    if(f == NULL){
        printf("Error opening Input file!");
        return NULL;
    }
    // Go after three long bytes (header)
    fseek(f, sizeof(long int) * 3, SEEK_SET);
    // perform reconstructing coding tree
    int index = 0;
    Tree* tree = reconstructTreeRecurse(f, &index);

    fclose(f);
    return tree;
}


int saveTreeCharacterBased(Tree* tree, char* filename){
    // open file for writing
    FILE* f;
    f = fopen(filename, "wb");
    if(f == NULL){
        printf("Error opening output file! '%s'", filename);
        return EXIT_FAILURE;
    }

    // recursively save tree
    saveTreeCharacterBasedRecurse(tree, f);

    fclose(f);
    return EXIT_SUCCESS;
}


int decodeFile(char* inputFilename, Tree* tree, char* outputFilename, long charFrequency[]){
    // open files for reading and writing
    FILE *inputFile, *outputFile;
    inputFile = fopen(inputFilename, "rb");
    outputFile = fopen(outputFilename, "w");

    fseek(inputFile, sizeof(long int), SEEK_SET);
    // get the size used to store the tree
    long int treeSize;
    if(fread(&treeSize, sizeof(long int), 1, inputFile) != 1){
        printf("Cannot read from inputfile!");
        return EXIT_FAILURE;
    }
    // get the numbers of characters to decode
    long int numCharTotal, numCharDecoded = 0;
    if(fread(&numCharTotal, sizeof(long int), 1, inputFile) != 1){
        printf("Cannot read from inputfile!");
        return EXIT_FAILURE;
    }
    // skip the tree content
    fseek(inputFile, treeSize, SEEK_CUR);

    // initialise variables
    Tree* curr = tree; char byte = 0; int bit, index = 0;
    // decode until the specified number of characters is decoded
    while(numCharDecoded < numCharTotal){
        // check for leaf node
        if(isLeafNode(curr)){
            // save node value
            byte = curr->value;
            fwrite(&byte, 1, 1, outputFile);
            // increase decoded character count
            numCharDecoded++;
            // reset curr to root
            curr = tree;
            // increase the frequency of curr character
            charFrequency[(int)byte]++;
        }
        // read a single byte from encoded file
        if(fread(&byte, 1, 1, inputFile) != 1) {
            printf("byte cannot be read from file.........%d, %ld\n", index, numCharDecoded);
            return EXIT_FAILURE;
        }
        // check bit value at index
        //printf("the byte value is ###### %d\n", byte);
        bit = getBitByIndex(byte, index);
        //printf("The index value is........%d\n", index);
        //printf("The bit value is %d\n", bit);
        // bit zero, go left
        if(bit == 0){
            curr = curr->left;
        }
        // bit one, go right
        if(bit == 1){
            curr = curr->right;
        }
        // increase index
        index++;
        // reset if index reaches max
        if(index > 7) index = 0;
        // decide whether want to go to previous byte
        if(index != 0) fseek(inputFile, -1, SEEK_CUR);
    }

    // close files
    fclose(inputFile); fclose(outputFile);

    return EXIT_SUCCESS;
}


int saveCharFreq(long charFrequency[], char* filename){
    // open file for writing frequencies
    FILE* f = fopen(filename, "w");
    if(f == NULL){
        printf("Cannot open output file '%s'", filename);
        return EXIT_FAILURE;
    }

    // iterate through the array and write to file
    long freq = 0;
    for(int i = 0; i < 256; i++){
        freq = charFrequency[i];
        fwrite(&freq, sizeof(long), 1, f);
    }

    // close file
    fclose(f);

    return EXIT_SUCCESS;
}


Tree* constructHuffmanTree(long charFrequency[]){
    // sorted list of trees
    TreeList* sortedTreeList = constructTreeList(charFrequency);
    // return when a single tree is left in list tree
    Tree *firstTree, *secondTree, *combinedTree;
    while(sortedTreeList->next != NULL){
        // remove first two trees from sorted list
        deleteFirstTwo(&sortedTreeList, &firstTree, &secondTree);
        // combine them into one tree
        combinedTree = combineTwoTrees(firstTree, secondTree);
        // insert back in sorted order
        sortedInsert(&sortedTreeList, combinedTree);
    }

    // resulting tree is the huffman tree
    Tree* huffmanCodingTree = sortedTreeList->tree;

    deleteTreeList(sortedTreeList);
    return huffmanCodingTree;
}


int saveBitsRequired(char* originalFileName, Tree* reconstructedTree, Tree* huffmanTree, char* outputFilename){
    // open output file for writing
    FILE* f = fopen(outputFilename, "wb");
    if(f == NULL){
        printf("Error opening output file '%s'", outputFilename);
        return EXIT_FAILURE;
    }

   // number of bits required to encode given file
    long bitsRequired;

    // calculate bits for input file using reconstructed tree
    bitsRequired = calculateBitsRequired(originalFileName, reconstructedTree);
    // save
    saveBits(bitsRequired, f);

    // calculate bits for input file using huffman tree
    bitsRequired = calculateBitsRequired(originalFileName, huffmanTree);
    // save
    saveBits(bitsRequired, f);

    fclose(f);
    return EXIT_SUCCESS;
}