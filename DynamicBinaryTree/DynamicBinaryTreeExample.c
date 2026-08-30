#include <stdio.h>

#include "DynamicBinaryTree.h"

DynamicBinaryTreeDef(IntTree, int);

void printInt(const int i) {
    printf("%d", i);
}

int cmpInt(const int a, const int b) {
    return a - b;
}

void InOrderPrint(IntTree *tree) {
    if (!(tree)->root)
        return;

    if ((tree)->root->left) {
        printf("(");
        IntTree leftTree = {.printFunc = (tree)->printFunc, .cmpFunc = (tree)->cmpFunc, .root = (tree)->root->left};
        InOrderPrint(&leftTree);
        printf(")<-");
    }
    (tree)->printFunc((tree)->root->value);
    if ((tree)->root->right) {
        printf("->(");
        IntTree rightTree = {.printFunc = (tree)->printFunc, .cmpFunc = (tree)->cmpFunc, .root = (tree)->root->right};
        InOrderPrint(&rightTree);
        printf(")");
    }
}

int main() {
    IntTree tree = {.printFunc = printInt, .cmpFunc = cmpInt};
    DYNAMIC_BINARY_TREE_ADD(&tree, 1);
    DYNAMIC_BINARY_TREE_ADD(&tree, 2);
    DYNAMIC_BINARY_TREE_ADD(&tree, 0);
    DYNAMIC_BINARY_TREE_ADD(&tree, 1);
    InOrderPrint(&tree);
}