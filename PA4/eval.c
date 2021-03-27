#include "tree.h"


/**
 * construct a binary tree from the input file.
 * return 0, if it fails. 1, otherwise.
 */
int constructTree(FILE *f, Tnode **tree) {
    int key, res, ans;
    char data;

    res = fread(&key, sizeof(int), 1, f);
    res += fread(&data, sizeof(char), 1, f);
    if (res != 2) return 0;

    Tnode *node = newNode(key);

    if (data == 2 || data == 3) {
        ans = constructTree(f, &node->left);
        if (ans != 1) return 0;
    }
    if (data == 1 || data == 3) {
        ans = constructTree(f, &node->right);
        if (ans != 1) return 0;
    }
    if (data > 3 || data < 0) return 0;

    *tree = node;
    return 1;
}


/**
 * return -1, if child is less than or equal to root.
 * return 1, if child is greater.
 * return 0, if child is NULL.
 */
int compareNodes(Tnode *root, Tnode *child) {
    if (child == NULL) return 0;
    return child->key <= root->key ? -1 : 1;
}


/**
 * return 1, if the given tree is a valid bst.
 * return 0, otherwise.
 */
int validBST(Tnode *tree) {
    if (tree == NULL) return 1;

    int res = 0;
    if (compareNodes(tree, tree->left) <= 0) res++;
    if (compareNodes(tree, tree->right) >= 0) res++;
    if (validBST(tree->left) == 1) res++;
    if (validBST(tree->right) == 1) res++;

    if (res != 4) return 0;
    return 1;
}


int heightBalancedRecurse(Tnode *node, int *h) {
    if (node == NULL) {
        *h = 0;
        return 1;
    }

    int h1 = 0, h2 = 0, res;

    res = heightBalancedRecurse(node->left, &h1);
    if (res != 1) return 0;

    res = heightBalancedRecurse(node->right, &h2);
    if (res != 1) return 0;

    if (abs(h1 - h2) > 1) return 0;

    *h = 1 + ((h1 > h2) ? h1 : h2);
    return 1;
}


int heightBalanced(Tnode *tree) {

    int h = 0;
    int res = heightBalancedRecurse(tree, &h);

    if (res != 1) return 0;
    return 1;
}


int eval(char *input_filename) {
    // output integers
    int res1 = 1, res2 = 1, res3 = 1;

    // open file
    FILE *f = fopen(input_filename, "rb");
    if (f == NULL) {
        res1 = -1;
        goto leave;
    }

    Tnode *tree = NULL;
    res1 = constructTree(f, &tree);
    if (res1 != 1) goto leave;

    // check for valid bst
    res2 = validBST(tree);

    // check for height balance
    res3 = heightBalanced(tree);

    leave:
    printf("%d,%d,%d\n", res1, res2, res3);
    if (res1 != 1) return EXIT_FAILURE;

    fclose(f);
    deleteTree(tree);
    return EXIT_SUCCESS;
}