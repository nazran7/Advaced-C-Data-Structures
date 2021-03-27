#include "main.h"


int main(int argc, char **argv) {

    if (argc < 2) return EXIT_FAILURE;

    if (strcmp(argv[1], "-b") == 0) {
        if (argc != 4) return EXIT_FAILURE;

        // construct balanced binary tree from input file
        return build(argv[2], argv[3]);

    } else if (strcmp(argv[1], "-e") == 0) {
        if (argc != 3) return EXIT_FAILURE;

        // evaluate the given input for the valid bbst
        return eval(argv[2]);

    } else return EXIT_FAILURE;

    return EXIT_SUCCESS;
}