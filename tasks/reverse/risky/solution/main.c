#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

typedef struct node {
    int value;
    struct node* left;
    struct node* right;
    struct node* parent;
} Node;

Node* root = NULL;
Node* current = NULL;

Node* MakeNode(int);
void MakeLinkLeft(Node*, Node*);
void MakeLinkRight(Node*, Node*);
int GetPathListSize(Node**);
int GetValue(Node*);
void __attribute__ ((constructor)) setupGraph(void);
void __attribute__ ((constructor)) initPaths(void);
void init(void);
void printLeftBranch(Node* root);
Node* findInGraph(Node*, int, int*);
int* GetPathToChar(Node* cur, int symbol);

int* path;
int idx;

struct correct_path {
    int size;
    int* vals;
};

struct correct_path* CorrectList[27];

// min, max
// x - min == max - x
// x - min = max - x
// x = max + min - x
void FillLeft(Node* start, int border, int end)
{
    Node* cur = start;
    int val = cur->value;
    
    if (end % 2 != 0) end -= 1;
    
    for (int i = 0; val != end; i++) {
        if (((val + border) / 2 ) != val)
            MakeLinkLeft(cur, MakeNode((val + border) / 2));
        else 
            break;

        cur = cur->left;
        if (cur == NULL)
            break;
        val = cur->value;
    }
};

void FillRight(Node* start, int border, int end)
{
    Node* cur = start;
    int val = cur->value;
    
    if (end % 2 != 0) end -= 1;

    for (int i = 0; val != end; i++) {
        if (((val + border) / 2 ) != val)
            MakeLinkRight(cur, MakeNode((val + border) / 2 ));
        else
            break;

        cur = cur->right;
        if (cur == NULL)
            break;
        val = cur->value;
    }
};

void __attribute__ ((constructor)) initPaths(void)
{
    struct correct_path* cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 2);
    cor->size = 2;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    CorrectList[0] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 3);
    cor->size = 3;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    CorrectList[1] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 85;
    cor->vals[4] = 82;
    CorrectList[2] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 6);
    cor->size = 6;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 121;
    cor->vals[4] = 124;
    cor->vals[5] = 122;
    CorrectList[3] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 3);
    cor->size = 3;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    cor->vals[2] = 43;
    CorrectList[4] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 109;
    cor->vals[4] = 112;
    CorrectList[5] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 2);
    cor->size = 2;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    CorrectList[6] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 4);
    cor->size = 4;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 85;
    CorrectList[7] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    cor->vals[4] = 100;
    CorrectList[8] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 4);
    cor->size = 4;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    CorrectList[9] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 3);
    cor->size = 3;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    cor->vals[2] = 43;
    CorrectList[10] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    cor->vals[4] = 100;
    CorrectList[11] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 6);
    cor->size = 6;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 109;
    cor->vals[4] = 106;
    cor->vals[5] = 107;
    CorrectList[12] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    cor->vals[4] = 100;
    CorrectList[13] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 2);
    cor->size = 2;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    CorrectList[14] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 4);
    cor->size = 4;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 85;
    CorrectList[15] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    cor->vals[4] = 100;
    CorrectList[16] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 4);
    cor->size = 4;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 121;
    CorrectList[17] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 97;
    cor->vals[4] = 100;
    CorrectList[18] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 6);
    cor->size = 6;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 109;
    cor->vals[4] = 112;
    cor->vals[5] = 113;
    CorrectList[19] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    cor->vals[2] = 43;
    cor->vals[3] = 49;
    cor->vals[4] = 52;
    CorrectList[20] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 2);
    cor->size = 2;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    CorrectList[21] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 6);
    cor->size = 6;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 91;
    cor->vals[3] = 85;
    cor->vals[4] = 82;
    cor->vals[5] = 83;
    CorrectList[22] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 4);
    cor->size = 4;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    cor->vals[2] = 43;
    cor->vals[3] = 49;
    CorrectList[23] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 55;
    cor->vals[2] = 43;
    cor->vals[3] = 49;
    cor->vals[4] = 52;
    CorrectList[24] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 109;
    cor->vals[4] = 106;
    CorrectList[25] = cor;

    cor = malloc(sizeof(struct correct_path));
    cor->vals = malloc(sizeof(int) * 5);
    cor->size = 5;
    cor->vals[0] = 79;
    cor->vals[1] = 103;
    cor->vals[2] = 115;
    cor->vals[3] = 121;
    cor->vals[4] = 124;
    CorrectList[26] = cor;
}

void __attribute__ ((constructor)) setupGraph(void)
{
    
    int left_border = 32;
    int right_border = 127;
    root = MakeNode((left_border + right_border)/2);
    
    // file left and right branch starts from root
    FillLeft(root, left_border, left_border);
    FillRight(root, right_border, right_border);

    Node* cur = root->left;
    int border = root->value;
    while (cur != NULL) {
        FillRight(cur, border, border);
        border = cur->value;        
        cur = cur->left;
    }

    cur = root->right;
    border = root->value;
    while (cur != NULL) {
        FillLeft(cur, border, border);
        border = cur->value;
        cur = cur->right;
    }

    for (int i = 32; i < 127; i++) {
        Node* res = findInGraph(root, i, NULL);
        if (res == NULL)
            continue;

        if (res->left == NULL) {
           if (res->value == res->parent->value + 1) {
               continue;
           } else {
               FillLeft(res, res->parent->value, res->parent->value);
           }
        }
        if (res->right == NULL) {
            if (res->value == res->parent->value + 1) {
               continue;
           } else {
               FillRight(res, res->parent->value, res->parent->value);
           }
        }
    }
};

void init(void)
{
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
}

// CUP{1nCRed1ble+Rever5+T45k}
// MD5: 9db929bc4639cff9aef5b3850322d29b
int main()
{
    init();

    for (int i = 0; i < 27; i++) {
        printf("%d:[", i);
        for (int c = 32; c < 127; c++) {

            int* path = GetPathToChar(root, c);
            if (path == NULL)
                continue;

            int invalid = 0;
            if (idx == CorrectList[i]->size) {
                for (int j = 0; j < idx; j++) {
                    if (CorrectList[i]->vals[j] != path[j]) {
                        invalid = 1;
                        break;
                    } 
                }
                if (!invalid) {
                    printf("'%c',", c);
                }
            } else {
                continue;
            }
        }
        printf("],");
    }

    return 0;
};

int* GetPathToChar(Node* cur, int symbol)
{
    int* Path = malloc(sizeof(int) * 127);
    idx = 0;

    Node* tmp = findInGraph(cur, symbol, Path);

    if (tmp == NULL) {
        return NULL;
    }

    return Path;
}

Node* findInGraph(Node* cur, int symbol, int* Path)
{
    if (cur == NULL)
        return NULL;
    
    if (cur->value == symbol)
        return cur;
    
    if (cur->value > symbol) {
        if (Path != NULL)
            Path[idx++] = cur->value;
        if (cur->left != NULL)
            return findInGraph(cur->left, symbol, Path);
        else
            return NULL;
    } else {
        if (Path != NULL)
            Path[idx++] = cur->value;
        if (cur->right != NULL)
            return findInGraph(cur->right, symbol, Path);
        else 
            return NULL;
    }
}
Node* MakeNode(int value)
{
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->value = value;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
};

void MakeLinkLeft(Node* src, Node* dst)
{
    src->left = dst;
    dst->parent = src;
};

void MakeLinkRight(Node* src, Node* dst)
{
    src->right = dst;
    dst->parent = src;
}

int GetValue(Node* node)
{
    return node->value;
};
