#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pack.h"


int isValidFilename (char *filename) {
    char bad_chars[] = "!@%^*~|";
    int invalid_found = 0;
    int i;
    for (i = 0; i < strlen(bad_chars); ++i) {
        if (strchr(filename, bad_chars[i]) != NULL) {
            invalid_found = 1;
            break;
        }
    }
    if (invalid_found == 1) {
        printf("Invalid file name");
        return 0;
    }
    return 1;
}


int main(int argc, char **argv){

    // check for arguments count
    if (argc != 5) return EXIT_FAILURE;

    // construct a strictly binary tree from the input file
    Tree *packTree = constructTree(argv[1]);
    if (packTree == NULL) return EXIT_FAILURE;
    printTree(packTree);

    /* check for valid filenames before each operation */
    int res = 0;
    // store the pre-order traversal
    if (isValidFilename(argv[2]) == 1) {
        res += savePreOrder(packTree, argv[2]);
    }
    // store the dimensions
    if (isValidFilename(argv[3]) == 1) {
        res += saveDimensions(packTree, argv[3]);
    }
    // store the packing
    if (isValidFilename(argv[4]) == 1) {
        res += savePacking(packTree, argv[4]);
    }

    deleteTree(packTree);
    if (res > 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}