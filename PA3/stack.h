#include "pack.h"


typedef struct _Stack {
    Tree *tree;
    struct _Stack *next;
} Stack;


void push(Stack **stack, Tree* tree);

Tree* pop(Stack **stack);

void deleteStack(Stack *stack);