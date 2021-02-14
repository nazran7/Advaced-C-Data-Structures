#include <stdio.h>
#include <stdlib.h>

#include "shell_list.h"
#include "list_of_list.h"


static Node* Create_New_Node(long value){
    // return the pointer to new node
    Node* new = (Node*)malloc(sizeof(Node));
    new->value = value;
    new->next = NULL;

    return new;
}


Node *List_Load_From_File(char *filename){
    // file open
    FILE *f;
    f = fopen(filename, "rb");
    if(f == NULL){
        printf("Error opening file!");
        return NULL;
    }

    // create head node
    long int num;
    int res, count;
    res = fread(&num, sizeof(long), 1, f);
    if(res == 1) count++;
    Node* head = Create_New_Node(num);

    // create list
    Node* curr = head;
    while(1){
        res = fread(&num, sizeof(long), 1, f);
        if(res == 0) break; // end of file
        if(res == 1) count++;
        curr->next = Create_New_Node(num);
        curr = curr->next;
    }

    return head;
}


int List_Save_To_File(char *filename, Node *list){
    // open file for writing
    FILE *f;
    f = fopen(filename, "wb");
    if(f == NULL){
        printf("File Write unsuccessful. Cannot open file!");
    }

    // write all long integers
    int count = 0, res;
    long num;
    Node* curr = list;
    while(curr != NULL){
        num = curr->value;
        res = fwrite(&num, sizeof(long), 1, f);
        if(res == 1) count++;
        curr = curr->next;
    }

    return count;
}


static int Get_List_Size(Node *list){
    Node* curr = list;
    int count = 0;
    while(curr != NULL){
        curr = curr->next;
        count++;
    }
    //printf("The size calc is %d\n", count);
    return count;
}


static Node* Generate_Seq(int size){
    // initialise the max gap value
    long int gap = 1;
    Node* sequence = NULL;
    while(gap < size){
        if(sequence == NULL){
            sequence = Create_New_Node(gap);
        }
        else{
            Node* new = Create_New_Node(gap);
            new->next = sequence;
            sequence = new;
        }
        gap = gap * 3 + 1;
    }
    return sequence;
}


static void delete_list(Node* head){
    Node *curr, *next;
    curr = head;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
}


static List* Create_New_List(Node* node){
    // return the pointer to new List
    List* new = (List*)malloc(sizeof(List));
    new->node = node;
    new->next = NULL;

    return new;
}


static void reverse(Node** head_ref)
{
    Node* prev = NULL;
    Node* current = *head_ref;
    Node* next = NULL;
    while (current != NULL) {
        // Store next
        next = current->next;
 
        // Reverse current node's pointer
        current->next = prev;
 
        // Move pointers one position ahead.
        prev = current;
        current = next;
    }
    *head_ref = prev;
}


static Node* sortedInsert(Node* head, Node* node, long* n_comp){
    Node* curr = head;
    Node* prev = NULL;
    while(curr != NULL){
        *n_comp = *n_comp + 1;
        if(curr->value < node->value){
            if(prev == NULL){
                node->next = curr;
                return node;
            }
            prev->next = node;
            node->next = curr;
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    if(prev == NULL) return head;
    prev->next = node;
    return head;
}


static List* Break_List(Node* list, long K, long* n_comp){

    // do a single loop to obtain head of the list
    Node* curr = list;
    Node* temp = curr->next;
    curr->next = NULL;
    List* head = Create_New_List(curr);
    curr = temp;
    List* start = NULL;
    List* prev_start = head;
    int i = 1;
    if(K == 1) i = 0;
    // iterate throught every node in list
    while(curr != NULL){
        temp = curr->next;
        curr->next = NULL;

        // insert each node to the ith sorted list, where i = loop count
        // and the loop count restarts after K counts
        if(i == 0){
            // loop restarts
            start = head;
            prev_start = NULL;
        }

        if(start == NULL){
            // create new list
            start = Create_New_List(curr);
            prev_start->next = start;
        }
        else{
            // insert node
            start->node = sortedInsert(start->node, curr, n_comp);
        }

        prev_start = start;
        start = start->next;
        i++;
        if(i == K) i = 0;

        curr = temp;
    }

    // reverse each sublist
    start = head;
    while(start != NULL){
        reverse(&start->node);
        start = start->next;
    }

    return head;
}


static Node* Combine_List(List* list, long K, int size){

    // do a single run to obtain start of result
    Node* result = list->node;
    list->node = list->node->next;
    result->next = NULL;
    Node* tail = result;
    List* curr = list->next;

    int curr_size = 0;
    while(curr_size < size){

        // iterate through each sublist
        while(curr != NULL){

            // add node from each list to result
            Node* node = curr->node;
            if(node != NULL){ 
                curr->node = curr->node->next;
                node->next = NULL;
                tail->next = node;
                tail = node;
            }

            curr_size++;
            curr = curr->next;
        }
        curr = list;

    }

    return result;
}


Node *List_Shellsort(Node *list, long *n_comp)
{
    // generate sequence
    int size = Get_List_Size(list);
    Node* sequence = Generate_Seq(size);
    Node* curr_seq = sequence;

    // iterate throught the gap sequence
    while(curr_seq != NULL){

        // break the list into K lists, where K = Gap(sequence.value)
        List* sublist = Break_List(list, curr_seq->value, n_comp);

        // combine the K lists into a single list
        list = Combine_List(sublist, curr_seq->value, size);

        curr_seq = curr_seq->next;
    }

    delete_list(sequence);
    return list;
}