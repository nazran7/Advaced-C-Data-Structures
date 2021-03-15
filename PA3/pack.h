typedef struct _Tree {
    int id;
    int width;
    int height;
    int xpos;
    int ypos;
    struct _Tree *left;     // or top
    struct _Tree *right;    // or bottom
} Tree;


void deleteTree(Tree *tree);

void printTree(Tree *tree);

Tree *constructTree(char *filename);

int savePreOrder(Tree *packTree, char *filename);

int saveDimensions(Tree *packTree, char *filename);

int savePacking(Tree *packTree, char *filename);