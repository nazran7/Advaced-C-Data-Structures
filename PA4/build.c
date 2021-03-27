#include "tree.h"


int height(Tnode *node) {
    if (node == NULL) return 0;

    if (node->balance < 0) {
        return 1 + height(node->right);
    } else if (node->balance > 0) {
        return 1 + height(node->left);
    } else {
        if (node->left != NULL) return 1 + height(node->left);
        else return 1 + height(node->right);
    }
}


Tnode *rightRotate(Tnode *y)
{
    Tnode *x = y->left;
    Tnode *T2 = x->right;
 
    // Perform rotation
    x->right = y;
    y->left = T2;
 
    // update balances
    y->balance = height(y->left) - height(y->right);
    x->balance = height(x->left) - height(x->right);
 
    // Return new root
    return x;
}


Tnode *leftRotate(Tnode *x)
{
    Tnode *y = x->right;
    Tnode *T2 = y->left;
 
    // Perform rotation
    y->left = x;
    x->right = T2;
 
    // update balances
    y->balance = height(y->left) - height(y->right);
    x->balance = height(x->left) - height(x->right);
 
    // Return new root
    return y;
}


Tnode *insertRecurse(Tnode *node, int key, int *h) {
    if (node == NULL) return newNode(key);

    // if height is increased, mark *h
    if (isLeaf(node) == 1) *h = 1;

    if (key <= node->key) {
        // insert at left
        node->left = insertRecurse(node->left, key, h);
        if (*h == 1) node->balance += 1;
    } else {
        // insert at right
        node->right = insertRecurse(node->right, key, h);
        if (*h == 1) node->balance -= 1;
    }

    // check if balance exceeded
    if (node->balance < 2 && node->balance > -2) return node;
    
    *h = 0;

    // Left Left Case
    if (node->balance > 1 && key <= node->left->key)
        return rightRotate(node);
 
    // Right Right Case
    if (node->balance < -1 && key > node->right->key)
        return leftRotate(node);
 
    // Left Right Case
    if (node->balance > 1 && key > node->left->key)
    {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }
 
    // Right Left Case
    if (node->balance < -1 && key <= node->right->key)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}


Tnode *insertNode(Tnode *node, int key)
{
    if (node == NULL) return(newNode(key));

    int h = 0;
    node = insertRecurse(node, key, &h);

    return node;
}


Tnode *maxValueNode(Tnode *node) {
    Tnode *curr = node;

    while (curr->right != NULL)
    {
        curr = curr->right;
    }
    return curr;
}


Tnode *deleteNode(Tnode* root, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE
  
    if (root == NULL)
        return root;
  
    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if ( key < root->key )
        root->left = deleteNode(root->left, key);
  
    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if( key > root->key )
        root->right = deleteNode(root->right, key);
  
    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            Tnode *temp = root->left ? root->left :
                                             root->right;
  
            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else {
                // copy contents of that one child
                root->key = temp->key;
                root->balance = temp->balance;
                root->left = temp->left;
                root->right = temp->right;
            }

            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // predecessor (largest in the left subtree)
            Tnode* temp = maxValueNode(root->left);
  
            // Copy the inorder predecessor's data to this node
            root->key = temp->key;
  
            // Delete the inorder predecessor
            root->left = deleteNode(root->left, temp->key);
        }
    }
  
    // If the tree had only one node then return
    if (root == NULL)
      return root;
  
    // STEP 2: UPDATE BALANCE OF THE CURRENT NODE
    root->balance = height(root->left) - height(root->right);
  
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = root->balance;
  
    // If this node becomes unbalanced, then there are 4 cases
  
    // Left Left Case
    if (balance > 1 && root->left->balance >= 0)
        return rightRotate(root);
  
    // Left Right Case
    if (balance > 1 && root->left->balance < 0)
    {
        root->left =  leftRotate(root->left);
        return rightRotate(root);
    }
  
    // Right Right Case
    if (balance < -1 && root->right->balance <= 0)
        return leftRotate(root);
  
    // Right Left Case
    if (balance < -1 && root->right->balance > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
  
    return root;
}


int constructBST(FILE *f, Tnode **tree) {
    Tnode *bst = *tree;
    // read until EOF
    int res = 0, key;
    char op;
    res += fread(&key, sizeof(int), 1, f);
    res += fread(&op, sizeof(char), 1, f);
    while (res == 2) {
        if (op == 'i') {
            // insert key into tree
            bst = insertNode(bst, key);
        } else if (op == 'd') {
            // delete key from tree
            bst = deleteNode(bst, key);
        } else {
            *tree = bst;
            return EXIT_FAILURE;
        }

        res = 0;
        res += fread(&key, sizeof(int), 1, f);
        res += fread(&op, sizeof(char), 1, f);
    }

    *tree = bst;
    if (res != 0) return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}


int saveBST(FILE *f, Tnode *tree) {
    if (tree == NULL) return EXIT_SUCCESS;

    int res = 0;
    
    int key = tree->key;
    res = fwrite(&key, sizeof(int), 1, f);
    if (res != 1) return EXIT_FAILURE;

    char b = 0;
    if (tree->left != NULL) b = b | 2;
    if (tree->right != NULL) b = b | 1;

    res = fwrite(&b, sizeof(char), 1, f);
    if (res != 1) return EXIT_FAILURE;

    res = saveBST(f, tree->left);
    if (res == EXIT_FAILURE) return EXIT_FAILURE;
    saveBST(f, tree->right);
    if (res == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}


int build(char *input_filename, char *output_filename) {
    // open input file for reading
    FILE *ipf = fopen(input_filename, "rb");
    if (ipf == NULL) {
        printf("%d\n", -1);
        return EXIT_FAILURE;
    }

    int res1, res2;

    // construct bbst
    Tnode *bst = NULL;
    res1 = constructBST(ipf, &bst);
    if (res1 == EXIT_FAILURE) printf("%d\n", 0);
    fclose(ipf);

    //printf("the height of the bst is %d\n", height(bst));
    //preOrder(bst); printf("\n");

    // store output
    // open file for writing
    FILE *of = fopen(output_filename, "wb");
    if (of == NULL) {
        printf("%d\n", -1);
        return EXIT_FAILURE;
    }
    // save
    res2 = saveBST(of, bst);
    if (res2 == EXIT_FAILURE) printf("%d\n", 0);
    fclose(of);


    deleteTree(bst);

    if (res1 == EXIT_FAILURE || res2 == EXIT_FAILURE) return EXIT_FAILURE;
    else printf("%d\n", 1);

    return EXIT_SUCCESS;
}