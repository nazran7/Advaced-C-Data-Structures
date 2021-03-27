#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hbt.h"


Tnode *newNode(int key);

void preOrder(Tnode *node);

int isLeaf(Tnode *node);

void deleteTree(Tnode *tree);