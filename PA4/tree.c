#include "tree.h"


Tnode *newNode(int key)
{
    Tnode* node = (Tnode*) malloc(sizeof(Tnode));
    node->key   = key;
    node->left   = NULL;
    node->right  = NULL;
    node->balance = 0;
    return(node);
}


void preOrder(Tnode *node) {
    if (node == NULL) return;

    printf("%d\t", node->key);
    preOrder(node->left);
    preOrder(node->right);
}


int isLeaf(Tnode *node) {
    return node->left == NULL && node->right == NULL;
}


void deleteTree(Tnode *tree) {
    if (tree == NULL) return;

    deleteTree(tree->left);
    deleteTree(tree->right);
    free(tree);
}
