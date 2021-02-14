#include <stdio.h>
#include <stdlib.h>

#include "shell_array.h"
#include "shell_list.h"

/*
static void print_array(long* arr, int size){
    printf("total: %d\nThe array elements are:\n\n", size);
    for(int i = 0; i < size; i++){
        printf("%ld\n", arr[i]);
    }
    printf("\n");
}*/

/*
static void print_list(Node* head){
    printf("The elements in the list are:\n\n");
    Node* curr = head;
    while (curr != NULL)
    {
        printf("%ld\n", curr->value);
        curr = curr->next;
    }
    
}*/


static void delete_list(Node* head){
    Node *curr, *next;
    curr = head;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
}


int main(int argc, char* argv[]){

    // array shell sort
    if(argv[1][1] == 'a'){
        // loading
        int size;
        long* arr = Array_Load_From_File(argv[2], &size);
        if(arr == NULL){
            return EXIT_FAILURE;
        }
        //printf("%d integers have been read from input\n", size);
        //print_array(arr, size);

        // sorting
        long n_comp;
        Array_Shellsort(arr, size, &n_comp);
        //print_array(arr, size);
        printf("%ld\n", n_comp);

        // saving
        Array_Save_To_File(argv[3], arr, size);
        //printf("%d integers have been written to output\n", res);

        free(arr);
    }

    // list shell sort
    if(argv[1][1] == 'l'){
        // loading
        Node* list = List_Load_From_File(argv[2]);
        //print_list(list);

        // sorting
        long n_comp = 0;
        list = List_Shellsort(list, &n_comp);
        //print_list(list);

        printf("%ld\n", n_comp);

        // saving
        List_Save_To_File(argv[3], list);
        //printf("\n%d integers have been written to output\n", count);

        // free up memory
        delete_list(list);
    }
    return 0;
}