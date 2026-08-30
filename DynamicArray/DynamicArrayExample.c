#include <stdio.h>
#include <time.h>

#define DYNAMIC_ARRAY_IMPLEMENTATION
#include "DynamicArray.h"

// the type can be any type, even structs
DefineDynamicArrayAndSlice(IntArray, int);

void printInt(const int *i) {
    i ? printf("%d", *i) : printf("(NULL)");
}

void IntArrayExample() {
    IntArray arr = {0};

    DynamicArrayAppend(&arr, 1);
    DynamicArrayAppend(&arr, 2);
    DynamicArrayAppend(&arr, 3);
    DynamicArrayAppend(&arr, 4);

#if false
    DynamicArrayPrintN(&arr); // will crash
#endif

    arr.printFunc = printInt;
    DynamicArrayPrintN(&arr); // will work

    // the array should be initialized like this if you wanna print
    // IntArray arr = {.printFunc = printInt};

    // manual print of the array
    printf("{");
    DynamicArrayForeach(int, num, &arr) {
        printf("( %d )", *num);
        if (num - arr.data < arr.count - 1)
            printf(", ");
    }
    printf("}\n");

    DynamicArrayClear(&arr);

    DynamicArrayAppend(&arr, 10);
    DynamicArrayAppend(&arr, 20);
    DynamicArrayAppend(&arr, 30);
    DynamicArrayAppend(&arr, 40);

    DynamicArrayPrintN(&arr);

    DynamicArrayForeach(int, num, &arr) {
        *num /= 10;
    }

    DynamicArrayPrintN(&arr);

    srand(time(0));
    while (!DynamicArrayIsEmpty(&arr)) {
        // removing saves the order of the array
        DynamicArrayRemoveAt(&arr, rand() % arr.count);
        DynamicArrayPrintN(&arr);
    }

    for (int i = 1; i <= 10; i++) {
        DynamicArrayAppend(&arr, i * 10);
    }

    DynamicArrayPrintN(&arr);

    DynamicArrayRemoveAt(&arr, -1); // will do nothing because the index is out of bounds
    DynamicArrayRemoveAt(&arr, 50); // will do nothing because the index is out of bounds
    DynamicArrayRemoveAt(&arr, 10); // will do nothing because the index is out of bounds

    DynamicArrayPrintN(&arr);

    DynamicArrayDestroy(&arr); // destroy frees the data and resets the count, capacity and printFunc

    DynamicArrayAppend(&arr, 0);

#if false
    DynamicArrayPrintN(&arr); // will crash
#endif
}

void IntArraySliceExample() {
    IntArray arr = {.printFunc = printInt};

    DynamicArrayAppend(&arr, 1);
    DynamicArrayAppend(&arr, 2);
    DynamicArrayAppend(&arr, 3);
    DynamicArrayAppend(&arr, 4);

    // lets say i want to pass to a function only a part of the array
    // i could make some fake IntArray with a pointer and length which would be bad
    // or i could use a Slice

    // index 1 is element 2 in arr, index 2 is 3, together the slice is [2, 3]
    IntArraySlice slice1 = DynamicArraySlice(&arr, 1, 2);
    DynamicArrayPrintN(&slice1);

    // slices are not modifiable, is it an array you can only look at

    // something like this wont even compile:
    //     ...
    //     DynamicArrayAppend(&slice1, 4);
    //     ...
    // because assigning to the pointer of the data
    // is an assignment to a const
    // (and it doesnt work because it doesnt have capacity too)

    // something like this will crash when running
    // due to the indices being out of bounds
#if false
    IntArraySlice slice2 = DynamicArraySlice(&arr, 0, 4);
    DynamicArrayPrintN(&slice2);
#endif
}

int main() {
    printf("\n----- IntArray Example -----\n");
    IntArrayExample();
    printf("\n----- IntArray Example -----\n");

    printf("\n----- IntArraySlice Example -----\n");
    IntArraySliceExample();
    printf("\n----- IntArraySlice Example -----\n");
}