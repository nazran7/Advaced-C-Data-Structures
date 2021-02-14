#include <stdio.h>
#include <stdlib.h>

#include "shell_array.h"

long *Array_Load_From_File(char *filename, int *size){
    // file open
    FILE *f;
    f = fopen(filename, "rb");
    if(f == NULL){
        printf("Error opening file!");
        *size = 0;
        return NULL;
    }

    //calculating file size and number of integers
    fseek(f, 0L, SEEK_END); 
    int file_size = ftell(f);
    int num_integers = file_size / sizeof(long);
    //printf("%d ok %d", file_size, num_integers);

    //close and reopen for reuse
    fclose(f);
    f = fopen(filename, "rb");
    if(f == NULL){
        printf("Error opening file!");
        *size = 0;
        return NULL;
    }

    // long int array initialisation
    long* arr = (long*)malloc(sizeof(long)*num_integers);
    if(num_integers == 0){
        *size = 0;
        return arr;
    }

    // read file and store in array
    int count = 0, res;
    for(int i = 0; i < num_integers; i++){
        res = fread(&arr[i], sizeof(long), 1, f);
        //int res = fscanf(f, )
        if(res == 1) count++;
    }

    *size = count; // save size
    fclose(f); // close file
    return arr;
}


int Array_Save_To_File(char *filename, long *array, int size){
    // open file for writing
    FILE *f;
    f = fopen(filename, "wb");
    if(f == NULL){
        printf("File Write unsuccessful. Cannot open file!");
    }

    // write to file
    int count = 0, res;
    for(int i = 0; i < size; i++){
        res = fwrite(&array[i], sizeof(long), 1, f);
        if(res == 1) count++;
    }

    fclose(f);
    return count;
}


void Array_Shellsort(long *array, int size, long *n_comp){
    // check array empty
    if(size == 0) return;
    int gap = 1, j, count = 0;
    long temp;

    // initialise the max gap value
    while(gap < size / 3){
        gap = gap * 3 + 1;
    }

    // run sort algorithm
    while(gap > 0)
    {
        //printf("#%d gap value ", gap);
        for (int i = gap; i < size; i += 1)
        {
            temp = array[i];
            for (j = i; j >= gap && array[j - gap] > temp; j -= gap)
            {
                count++;
                array[j] = array[j - gap];
            }
            array[j] = temp;
        }
        gap = (gap - 1) / 3;
    }
    *n_comp = count;
}