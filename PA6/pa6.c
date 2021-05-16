#include "pa6.h"

int getSize(FILE *f, short int *m, short int *n)
{
    int res;

    res = fread(m, sizeof(short int), 1, f);
    if (res != 1)
        return EXIT_FAILURE;

    res = fread(n, sizeof(short int), 1, f);
    if (res != 1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

void displayList(Node **list, short int numRow, short int numCol)
{
    Node *curr = NULL;
    printf("The adjacency list is:\n\n");
    for (int i = 0; i < ((numRow + 1) * numCol) + 1; i++)
    {
        printf("Nodes adjacent to %-3d are: ", i);
        curr = list[i];
        if (curr == NULL)
            printf("NULL");
        while (curr != NULL) {
            printf(" %-3d", curr->index);
            curr = curr->next;
        }
        printf("\n");
    }
    printf("\n");
}

Node *createNewNode(int idx, int value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->index = idx;
    node->next = NULL;
    node->value = value;

    return node;
}

void addToList(Node **list, int idx, int value) {
    Node *newNode = createNewNode(idx, value);
    if (*list == NULL) {
        *list = newNode;
        return;
    }
    newNode->next = *list;
    *list = newNode;
    return;
}

void updateList(Node **list, short int numRow, short int numCol, int i, int j, short int value)
{
    int row = i * numCol, col = j;
    int index = row + col;
    // top
    if (i - 1 >= 0)
        addToList(&list[index], ((i - 1) * numCol) + col, value);
    // bottom
    if (i + 1 < numRow)
        addToList(&list[index], ((i + 1) * numCol) + col, value);
    // left
    if (j - 1 >= 0)
        addToList(&list[index], row + (col - 1), value);
    // right
    if (j + 1 < numCol)
        addToList(&list[index], row + (col + 1), value);
}

/**
 * there will be (numRow * numCol) no of vertices.
 * the value at the given table will be considered as the
 * weight of the edges that are leaving from the current node.
 * i.e if node at (1, 1) has a value 15,
 * the weight of all the edges that are going away from the node is 15.
 */
int constructGraph(FILE *inputFile, short int numRow, short int numCol, Node **graph)
{
    // initialise all values in adjacent list as NULL
    for (int i = 0; i < ((numRow + 1) * numCol) + 1; i++)
    {
        graph[i] = NULL;
    }

    short int value;
    int res = 0;
    // read file and fill up the array
    for (int i = 1; i < numRow + 1; i++)
    {
        for (int j = 0; j < numCol; j++)
        {
            // read value
            res = fread(&value, sizeof(short int), 1, inputFile);
            if (res != 1)
                return EXIT_FAILURE;
            // update the list
            updateList(graph, numRow + 1, numCol, i, j, value);
        }
    }
    for (int i = numRow * numCol; i < (numRow + 1) * numCol; i++)
    {
        //graph[(numRow + 1) * numCol][i] = 0;
        addToList(&graph[(numRow + 1) * numCol], i, 0);
    }

    displayList(graph, numRow, numCol);
    return EXIT_SUCCESS;
}

/**
 * go through the adjacency matrix and print to file the weights on each row.
 */
int generateTextFile(FILE *outputFile, Node **graph, short int numRow, short int numCol)
{

    fprintf(outputFile, "%hd %hd\n", numRow, numCol);

    short int value;
    for (int i = numCol; i < (numRow + 1) * numCol; i++)
    {
        value = graph[i]->value;
        if ((i + 1) % numCol != 1)
            fprintf(outputFile, " ");
        fprintf(outputFile, "%hd", value);
        if ((i + 1) % numCol == 0)
            fprintf(outputFile, "\n");
    }

    return EXIT_SUCCESS;
}

void swap(int a, int b, int *heap) {
    int temp = heap[a];
    heap[a] = heap[b];
    heap[b] = temp;
}

int compare(int a, int b, int *distance, int *pathLength) {
    if (distance[a] > distance[b]) return 1;
    if (distance[a] < distance[b]) return -1;

    // equal distance
    if (pathLength[a] > pathLength[b]) return 1;
    if (pathLength[a] < pathLength[b]) return -1;
    return -1;
}

void heapify(int idx, int *heap, int *loc, int *distance, int *size, int *pathLength) {
    int s = idx;
    int l = 2 * s + 1;
    int r = 2 * s + 2;

    if (l < *size && compare(heap[l], heap[s], distance, pathLength) == -1) {
        s = l;
    }
    if (r < *size && compare(heap[r], heap[s], distance, pathLength) == -1) {
        s = r;
    }

    if (s != idx) {
        loc[heap[s]] = idx;
        loc[heap[idx]] = s;

        swap(s, idx, heap);
        heapify(s, heap, loc, distance, size, pathLength);
    }
}

int findNode(int *heap, int *loc, int *distance, int *size, int *pathLength)
{
    int index = heap[0];
    //printf("%d heap first \n", heap[0]);

    int last = heap[*size - 1];
    heap[0] = last;
    loc[last] = 0;
    loc[index] = *size - 1;

    *size = *size - 1;
    heapify(0, heap, loc, distance, size, pathLength);

    return index;
}

void updateHeap(int idx, int *heap, int *loc, int *distance, int *pathLength) {
    int i = loc[idx];
    //printf("the distance of 19: %d\n", heap[9]);
    while (i > 0 && compare(heap[i], heap[(int)((i - 1) / 2)], distance, pathLength) == -1) {
        //printf("%d %d happening...\n", i, (int)((i - 1) / 2));
        loc[heap[i]] = (int)((i - 1) / 2);
        loc[heap[(int)((i - 1) / 2)]] = i;
        swap(i, (int)((i - 1)/2), heap);

        i = (int)((i - 1) / 2);
    }
}

/**
 * iterate through the adjacent vertices and update distance,
 * if the new distance is less than the previous.
 */
void updateDistance(int u, Node **graph, int *heap, int *loc, int *distance, int *visit, int *parent, int *pathLength)
{
    int newDistance;
    Node *curr = graph[u];
    while (curr != NULL)
    {
        if (visit[curr->index] == 0) {
            //printf("%d %d %d %d this\n", distance[u], u, curr->value, curr->index);
            newDistance = distance[u] + curr->value;
            if (distance[u] != INT_MAX && newDistance < distance[curr->index])
            {
                parent[curr->index] = u;
                pathLength[curr->index] = pathLength[u] + 1;
                distance[curr->index] = newDistance;
                updateHeap(curr->index, heap, loc, distance, pathLength);
                //printf("After heap update: %d\n", loc[curr->index]);
            }
        }
        curr = curr->next;
    }
}

/**
 * find the shortest distance of every node from the source node
 * using dijkstra's algorithm.
 */
void findFastestTimesAndPath(Node **graph, int *shortestDistance, int numVertices, int *parent)
{
    // allocate path length array
    int *pathLength = (int *)malloc(numVertices * sizeof(int));
    // initialise pathlength as max
    for (int i = 0; i < numVertices; i++)
    {
        pathLength[i] = INT_MAX;
    }
    // make pathlength for source as 0
    pathLength[numVertices - 1] = 0;

    // allocate heap
    int *heap = (int *)malloc(numVertices * sizeof(int));
    // initialise heap
    for (int i = 0; i < numVertices; i++)
    {
        heap[i] = i;
    }
    // make last node as root
    heap[0] = numVertices - 1;
    heap[numVertices - 1] = 0;

    // allocate location array
    int *loc = (int *)malloc(numVertices * sizeof(int));
    // initialise locations
    for (int i = 0; i < numVertices; i++)
    {
        loc[i] = i;
    }
    // make loc of last node as first
    loc[numVertices - 1] = 0;
    loc[0] = numVertices - 1;

    // keep track of vertices that are visited
    int *visit = (int *)malloc(numVertices * sizeof(int));
    // intialise to 0, meaning not visited
    for (int i = 0; i < numVertices; i++)
        visit[i] = 0;
    // count the processed vertices
    int size = numVertices;
    // process all vertices
    int curr;
    while (size > 0)
    {
        // find the minimum distance not visited node
        curr = findNode(heap, loc, shortestDistance, &size, pathLength);
        if (curr == -1)
        {
            printf("\nsomethings wrong\n");
            break;
        }
        // mark as visited
        visit[curr] = 1;
        // update distances for adjacent vertices
        updateDistance(curr, graph, heap, loc, shortestDistance, visit, parent, pathLength);
    }

    free(visit);
    free(heap);
    free(loc);
    free(pathLength);
}

int generateFastestTimesFile(FILE *outputFile, Node **graph, short int numRow, short int numCol, int **fastestPath, int *result)
{
    int res = 0;
    res = fwrite(&numCol, sizeof(short int), 1, outputFile);
    if (res != 1)
        return EXIT_FAILURE;

    int shortestSrc = -1, shortestTime = INT_MAX;

    // calculate the number of vertices in the graph
    short int numVertices = (numRow + 1) * numCol + 1;

    // allocate array for storing duration of each vertices from source
    int *fastestTimes = (int *)malloc(numVertices * sizeof(int));
    // initialise fastestTime for each node as infinite
    for (int i = 0; i < numVertices; i++)
        fastestTimes[i] = INT_MAX;
    // make the time to reach source as 0, here source is the bottom most node
    fastestTimes[numVertices - 1] = 0;

    // allocate the path array for storing the parent of each vertices
    int *path = (int *)malloc(numVertices * sizeof(int));
    // initialise path for each node as -1
    for (int i = 0; i < numVertices; i++)
        path[i] = -1;

    // find fastestTime and path for every other node using Dijkstra algorithm
    findFastestTimesAndPath(graph, fastestTimes, numVertices, path);

    // write to file the fastest time of exit for each column
    for (int i = 0; i < numCol; i++)
    {
        printf("%-3d is the exit time for src %-2d\n", fastestTimes[i], i);
        fwrite(&fastestTimes[i], sizeof(int), 1, outputFile);

        // store the shortest details for the 3rd output file
        if (fastestTimes[i] < shortestTime)
        {
            shortestTime = fastestTimes[i];
            shortestSrc = i;
        }
    }

    *fastestPath = path;

    printf("\nthe shortest of all src is %d\nthe shortest of all time is %d\n", shortestSrc, shortestTime);
    result[0] = shortestSrc;
    result[1] = shortestTime;

    free(fastestTimes);
    return EXIT_SUCCESS;
}

int findLength(int *path, int start)
{
    int count = 0;
    while (path[start] != -1)
    {
        count++;
        start = path[start];
        //printf("#%d#", start);
    }
    return count - 1;
}

void writePath(FILE *file, int *path, int start, short int numCol)
{
    if (path[start] == -1)
    {
        return;
    }
    // write the current location to file
    short int row = (start / numCol) - 1;
    short int col = start % numCol;
    printf("(%d, %d)\n", row, col);
    fwrite(&row, sizeof(short int), 1, file);
    fwrite(&col, sizeof(short int), 1, file);

    writePath(file, path, path[start], numCol);
}

int generateFastestPathFile(FILE *outputFile, int *path, int *result, short int numCol)
{
    int res;
    res = fwrite(&result[1], sizeof(int), 1, outputFile);
    if (res != 1)
        return EXIT_FAILURE;

    // find the length of the path
    int pathLength = findLength(path, result[0]);
    printf("the length of the path is: %d\n", pathLength);
    res = fwrite(&pathLength, sizeof(int), 1, outputFile);

    // write the path
    writePath(outputFile, path, path[result[0]], numCol);
    return EXIT_SUCCESS;
}

void deleteList(Node *list) {
    if (list == NULL)
        return;
    deleteList(list->next);
    free(list);
    return;
}

void deleteGraph(Node **graph, int numVertices)
{
    for (int i = 0; i < numVertices; i++)
    {
        //free(graph[i]);
        deleteList(graph[i]);
    }

    free(graph);
}

int main(int argc, char const **argv)
{
    // check argument count
    if (argc != 5)
    {
        return EXIT_FAILURE;
    }
    FILE *f = NULL, *tf = NULL, *ff = NULL, *fp = NULL;
    // open file
    f = fopen(argv[1], "rb");
    if (f == NULL)
    {
        return EXIT_FAILURE;
    }

    int res = 0;

    // get list size
    short int numRow = 0, numCol = 0;
    res = getSize(f, &numRow, &numCol);
    if (res != 0)
    {
        fclose(f);
        return EXIT_FAILURE;
    }
    printf("The size of the table is: %d x %d\n", numRow, numCol);

    // allocate the graph -> adjacency list representation
    // an extra top row and an extra fake source vertex at the bottom
    int numVertices = (((numRow + 1) * numCol) + 1);
    Node **graph = (Node **)malloc(numVertices * sizeof(Node *));

    // construct graph from input data
    res = constructGraph(f, numRow, numCol, graph);
    if (res != 0)
        goto safe_exit;

    // output text file
    // open file
    tf = fopen(argv[2], "w");
    if (tf == NULL)
    {
        res = EXIT_FAILURE;
        goto safe_exit;
    }
    res = generateTextFile(tf, graph, numRow, numCol);
    if (res != 0)
        goto safe_exit;

    // fastest time file
    // open file
    ff = fopen(argv[3], "wb");
    if (ff == NULL)
    {
        res = EXIT_FAILURE;
        goto safe_exit;
    }

    int *fastestPath = NULL;
    int result[2];
    res = generateFastestTimesFile(ff, graph, numRow, numCol, &fastestPath, result);
    if (res != 0)
        goto safe_exit;

    // fastest path file
    // open file
    fp = fopen(argv[4], "wb");
    if (fp == NULL)
    {
        res = EXIT_FAILURE;
        goto safe_exit;
    }
    res = generateFastestPathFile(fp, fastestPath, result, numCol);
    if (res != 0)
        goto safe_exit;

safe_exit:
    fclose(f);
    fclose(tf);
    fclose(ff);
    fclose(fp);
    free(fastestPath);
    deleteGraph(graph, numVertices);

    if (res != 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
