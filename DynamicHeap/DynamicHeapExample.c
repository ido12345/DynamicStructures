#include <stdio.h>
#include <string.h>

#define DYNAMIC_HEAP_IMPLEMENTATION
#include "DynamicHeap.h"

DefineDynamicHeap(StringHeap, const char *);

int cmpStrMin(const char *a, const char *b) {
    return strcmp(a, b);
}

int cmpStrMax(const char *a, const char *b) {
    return strcmp(b, a);
}

void printStr(const char *str) {
    if (str)
        printf("\"%s\"", str);
    else
        printf("(null)");
}

// chatGPT generated example
int main() {
    printf("\n----------------- MinHeap Example -----------------\n");

    StringHeap minHeap = {.printFunc = printStr, .cmpFunc = cmpStrMin};

    // Inserting strings
    DynamicHeapInsert(&minHeap, "Orange");
    DynamicHeapInsert(&minHeap, "Apple");
    DynamicHeapInsert(&minHeap, "Banana");
    DynamicHeapInsert(&minHeap, "Grapes");

    printf("Heap after insertions: ");
    DynamicHeapPrintN(&minHeap);

    // Get head (min element)
    const char *head = DynamicHeapGetHead(&minHeap);
    printf("Head of heap (min): %s\n", head);

    // Extract head (min element)
    head = DynamicHeapExtractHead(&minHeap);
    printf("Extracted head: %s\n", head);

    printf("Heap after extracting head: ");
    DynamicHeapPrintN(&minHeap);

    // Remove a specific element
    DynamicHeapRemove(&minHeap, "Banana");
    printf("Heap after deleting \"Banana\": ");
    DynamicHeapPrintN(&minHeap);

    // Clear heap
    DynamicHeapClear(&minHeap);
    printf("Heap after clearing: ");
    DynamicHeapPrintN(&minHeap);

    // Destroy heap
    DynamicHeapDestroy(&minHeap);

    printf("\n----------------- MinHeap Example -----------------\n");
    printf("\n----------------- MaxHeap Example -----------------\n");

    StringHeap maxHeap = {.printFunc = printStr, .cmpFunc = cmpStrMax};

    DynamicHeapInsert(&maxHeap, "Alpha");
    DynamicHeapInsert(&maxHeap, "Zulu");
    DynamicHeapInsert(&maxHeap, "Echo");
    DynamicHeapInsert(&maxHeap, "Delta");

    printf("Max Heap after insertions: ");
    DynamicHeapPrintN(&maxHeap);

    int idx = {};
    DynamicHeapIndexOf(&maxHeap, "Echo", idx);
    printf("Index of \"Echo\" in heap: %d\n", idx);

    const char *max = DynamicHeapExtractHead(&maxHeap);
    printf("Extracted head (max): %s\n", max);

    printf("Max Heap after extracting head: ");
    DynamicHeapPrintN(&maxHeap);

    DynamicHeapDestroy(&maxHeap);

    printf("\n----------------- MaxHeap Example -----------------\n");
}
