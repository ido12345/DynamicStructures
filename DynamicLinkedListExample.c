#include <stdio.h>

#define DYNAMIC_LINKED_LIST_IMPLEMENTATION
#include "DynamicLinkedList.h"

// this also creates a new struct called <name>Node (in this case its IntListNode) that is a single node of the list
DefineDynamicLinkedList(IntList, int);

void printInt(const int *i) {
    i ? printf("%d", *i) : printf("(null)");
}

// #define repeat(c) for (int __r_i = 0; __r_i < (c); __r_i++)

int main() {
    IntList list = {.printFunc = printInt};
    printf("--------------------------------------------------\n");

    DynamicLinkedListInsertAt(&list, 0, 3);
    DynamicLinkedListInsertAt(&list, 0, 2);
    DynamicLinkedListInsertAt(&list, 0, 1);

    DynamicLinkedListPrintN(&list);

    DynamicLinkedListAppend(&list, 4);
    DynamicLinkedListAppend(&list, 5);
    DynamicLinkedListAppend(&list, 6);

    DynamicLinkedListPrintN(&list);

    printf("--------------------------------------------------\n");

    int insertIdx = 3;

    DynamicLinkedListInsertAt(&list, insertIdx, 100);

    DynamicLinkedListPrintN(&list);

    DynamicLinkedListRemoveAt(&list, insertIdx);

    DynamicLinkedListPrintN(&list);

    printf("--------------------------------------------------\n");

    DynamicLinkedListForeach(IntListNode, node, &list) {
        printf("|_%d_|->", node->value);
    }
    printf("\n");

    printf("--------------------------------------------------\n");

    int count = DynamicLinkedListCount(&list);
    printf("count = %d\n", count);

    printf("--------------------------------------------------\n");

    int getIdx = 3;
    IntListNode *node = DynamicLinkedListGetAt(&list, getIdx);
    printf("idx %d: [%d]\n", getIdx, node->value);
    IntListNode *firstNode = DynamicLinkedListGetFirst(&list);
    printf("first: [%d]\n", firstNode->value);
    IntListNode *lastNode = DynamicLinkedListGetLast(&list);
    printf("last:  [%d]\n", lastNode->value);

    printf("--------------------------------------------------\n");

    DynamicLinkedListClear(&list);

    DynamicLinkedListPrintN(&list);

    DynamicLinkedListDestroy(&list);

    return 0;
}