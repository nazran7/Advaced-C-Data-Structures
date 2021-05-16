#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct _Node
{
    int index;
    int value;
    struct _Node *next;
} Node;