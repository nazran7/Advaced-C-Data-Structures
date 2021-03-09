typedef struct _Tree
{
    char value;
    long int weight;
    struct _Tree *left;
    struct _Tree *right;
} Tree;


typedef struct _TreeList
{
    Tree* tree;
    struct _TreeList* next;
} TreeList;


void deleteTree(Tree* tree);

void displayTreePreOrder(Tree* tree);

Tree* reconstructTree(char* filename);

int saveTreeCharacterBased(Tree* tree, char* filename);

int decodeFile(char* inputFilename, Tree* tree, char* outputFilename, long charFrequency[]);

int saveCharFreq(long charFrequency[], char* filename);

Tree* constructHuffmanTree(long charFrequency[]);

int saveBitsRequired(char* originalFileName, Tree* reconstructedTree, Tree* huffmanTree, char* outputFilename);