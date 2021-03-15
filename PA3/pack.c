#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"


/****************** HELPER FUNCTIONS ****************/


Tree *createNewTree() {
    Tree *newTree = (Tree*) malloc(sizeof(Tree));
    newTree->id = 0;
    newTree->width = 0;
    newTree->height = 0;
    newTree->xpos = -1;
    newTree->ypos = -1;
    newTree->left = NULL;
    newTree->right = NULL;
    return newTree;
}


void deleteTree(Tree *tree) {
    if (tree == NULL) return;
    Tree* left = tree->left;
    Tree* right = tree->right;
    free(tree);
    deleteTree(left);
    deleteTree(right);
}


void printTree(Tree *tree) {
    if (tree == NULL) return;
    if (tree->left != NULL && tree->right != NULL) {
        printf("%c(%d,%d)\n", tree->id, tree->width, tree->height);
        printTree(tree->left);
        printTree(tree->right);
        return;
    }
    printf("%d(%d,%d)\n", tree->id, tree->width, tree->height);
}


int savePreOrderRecurse(Tree *tree, FILE *file) {
    if (tree == NULL) return 1;

    int res = 0, out;

    if (tree->left != NULL && tree->right != NULL) {
        out = fprintf(file, "%c\n", tree->id);
        //printf("The value of out(1): %d", out);
        if (out != 2) return 1;

        res += savePreOrderRecurse(tree->left, file);
        res += savePreOrderRecurse(tree->right, file);
        return res;
    }

    out = fprintf(file, "%d(%d,%d)\n", tree->id, tree->width, tree->height);
    //printf("The value of out(2): %d", out);
    if (out < 7) return 1;

    return 0;
}


int saveDimensionsRecurse(Tree *tree, FILE *file) {
    if (tree == NULL) return 1;

    int res = 0, out;

    if (tree->left != NULL && tree->right != NULL) {

        res += saveDimensionsRecurse(tree->left, file);
        res += saveDimensionsRecurse(tree->right, file);

        out = fprintf(file, "%c(%d,%d)\n", tree->id, tree->width, tree->height);
        //printf("The value of out(1): %d", out);
        if (out < 7) return 1;

        return res;
    }

    out = fprintf(file, "%d(%d,%d)\n", tree->id, tree->width, tree->height);
    //printf("The value of out(2): %d", out);
    if (out < 7) return 1;

    return 0;
}


void calculatePacking(Tree *tree, int xref, int yref) {
    if (tree == NULL) return;
    tree->xpos = xref;
    tree->ypos = yref;
    if (tree->left != NULL && tree->right != NULL) {
        if (((char) tree->id) == 'H') {     // H cut
            // top
            calculatePacking(tree->left, xref, yref + tree->right->height);
            // bottom
            calculatePacking(tree->right, xref, yref);
        } else {    // V cut
            // left
            calculatePacking(tree->left, xref, yref);
            // right
            calculatePacking(tree->right, xref + tree->left->width, yref);
        }
    }
    return;
}


int savePackingRecurse(Tree *tree, FILE *file) {
    if (tree == NULL) return 1;

    int res = 0, out;

    if (tree->left != NULL && tree->right != NULL) {

        res += savePackingRecurse(tree->left, file);
        res += savePackingRecurse(tree->right, file);

        return res;
    }

    out = fprintf(file, "%d((%d,%d)(%d,%d))\n",
                  tree->id, tree->width, tree->height,
                  tree->xpos, tree->ypos);
    //printf("The value of out(2): %d", out);
    if (out < 14) return 1;

    return 0;
}


/****************** MAIN FUNCTIONS ****************/


Tree *constructTree(char *filename) {
    // open file for reading
    FILE *f = fopen(filename, "r");
    if (f == NULL) return NULL;

    // variables to use
    int res1, res2;
    char line[20];
    Tree *left, *right;
    Stack *stack = NULL;

    // read line by line from file
    res1 = fscanf(f, "%s", line);
    while (res1 == 1) {
        // create a new tree for each line
        Tree *new = createNewTree();
        // check if leaf or not a leaf
        if (strlen(line) == 1) {    // not a leaf
            res2 = sscanf(line, "%lc", &new->id);
            if (res2 != 1) goto safe_exit;
            // pop last two from stack
            right = pop(&stack); left = pop(&stack);
            // add as left & righ to current node
            new->left = left; new->right = right;
            // add width & height of the new node
            if (((char) new->id) == 'H') {  // horizontal cut
                new->width = left->width > right->width ? left->width : right->width;
                new->height = left->height + right->height;
            } else if (((char) new->id) == 'V'){    // vertical cut
                new->height = left->height > right->height ? left->height : right->height;
                new->width = left->width + right->width;
            } else goto safe_exit;
            // push back the new node
            push(&stack, new);
        } else {    // leaf
            res2 = sscanf(line, "%d(%d,%d)", &new->id, &new->width, &new->height);
            if (res2 != 3) goto safe_exit;
            // push the new node to stack
            push(&stack, new);
        }
        // readline
        res1 = fscanf(f, "%s", line);
    }

    // if the stack has more than one element or empty, error
    if (stack == NULL || stack->next != NULL) goto safe_exit;
    // only tree present is the required tree
    Tree* tree = stack->tree;
    free(stack);
    fclose(f);
    return tree;

    safe_exit:
        printf("Safe Exiting..");
        deleteStack(stack);
        fclose(f);
        return NULL;
}


int savePreOrder(Tree *packTree, char *filename) {
    // open file
    FILE *f = fopen(filename, "w");
    if (f == NULL) return EXIT_FAILURE;

    // save in file recursively
    int res = savePreOrderRecurse(packTree, f);
    //printf("the value of res: %d", res);
    if (res > 0) {
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}


int saveDimensions(Tree *packTree, char *filename) {
    // open file for writing
    FILE *f = fopen(filename, "w");
    if (f == NULL) return EXIT_FAILURE;

    // save recursively
    int res = saveDimensionsRecurse(packTree, f);
    if (res > 0) {
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}


int savePacking(Tree *packTree, char *filename) {
    // calculate & assign co-ordinates to all blocks
    calculatePacking(packTree, 0, 0);

    // open file for writing
    FILE *f = fopen(filename, "w");
    if (f == NULL) return EXIT_FAILURE;

    // store the pack in post order recurse
    int res = savePackingRecurse(packTree, f);
    if (res > 0) {
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}