// 2025/5/23: i decided binary trees are kinda very useless and most functions require recursion to use it so for now its deprecated

#ifndef _DYNAMIC_BINARY_TREE_H
#define _DYNAMIC_BINARY_TREE_H

#include <stdio.h>
#include <stdlib.h>

#define NodeName(name) name##Node

#define DynamicBinaryTreeNode(name, type) \
    struct NodeName(name) {               \
        struct NodeName(name) * left;     \
        struct NodeName(name) * right;    \
        type value;                       \
    }

#define DynamicBinaryTree(name, type)           \
    struct {                                    \
        struct NodeName(name) * root;           \
        void (*printFunc)(const type);          \
        int (*cmpFunc)(const type, const type); \
    }

#define DynamicBinaryTreeNodeDef(name, type) typedef DynamicBinaryTreeNode(name, type) NodeName(name)

#define DynamicBinaryTreeDef(name, type)  \
    DynamicBinaryTreeNodeDef(name, type); \
    typedef DynamicBinaryTree(name, type) name

#define DYNAMIC_BINARY_TREE_ADD(bst, x)                      \
    do {                                                     \
        if (!(bst)->root) {                                  \
            (bst)->root = malloc(sizeof(*(bst)->root));      \
            (bst)->root->left = NULL;                        \
            (bst)->root->right = NULL;                       \
            (bst)->root->value = (x);                        \
        } else {                                             \
            typeof((bst)->root) temp = (bst)->root;          \
            while (temp) {                                   \
                if ((bst)->cmpFunc((x), temp->value) < 0) {  \
                    if (!temp->left) {                       \
                        temp->left = malloc(sizeof(*temp));  \
                        temp->left->left = NULL;             \
                        temp->left->right = NULL;            \
                        temp->left->value = (x);             \
                        break;                               \
                    }                                        \
                    temp = temp->left;                       \
                } else {                                     \
                    if (!temp->right) {                      \
                        temp->right = malloc(sizeof(*temp)); \
                        temp->right->left = NULL;            \
                        temp->right->right = NULL;           \
                        temp->right->value = (x);            \
                        break;                               \
                    }                                        \
                    temp = temp->right;                      \
                }                                            \
            }                                                \
        }                                                    \
    } while (0)

#endif