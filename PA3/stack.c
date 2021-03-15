#include <stdlib.h>

#include "stack.h"


Stack *createNewStack(Tree *tree) {
    Stack *newStack = (Stack*) malloc(sizeof(Stack));
    newStack->tree = tree;
    newStack->next = NULL;
    return newStack;
}


void push(Stack **stack, Tree *tree) {
    Stack *new = createNewStack(tree);
    Stack *head = *stack;
    if (head == NULL) {
        *stack = new;
        return;
    }
    new->next = head;
    *stack = new;
    return;
}


Tree *pop(Stack **stack) {
    Stack *head = *stack;
    if (head == NULL) return NULL;
    Tree* tree = head->tree;
    *stack = head->next;
    free(head);
    return tree;
}


void deleteStack(Stack *stack) {
    if (stack == NULL) return;
    Stack *next = stack->next;
    Tree *tree = stack->tree;
    free(stack);
    deleteTree(tree);
    deleteStack(next);
}