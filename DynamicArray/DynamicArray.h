#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef DYNAMIC_ARRAY_INT
    #define DYNAMIC_ARRAY_INT int32_t
#endif // DYNAMIC_ARRAY_INT
typedef DYNAMIC_ARRAY_INT DA_Int;

#define DynamicArrayStruct(type)         \
    struct {                             \
        type *data;                      \
        DA_Int count;                    \
        DA_Int capacity;                 \
        void (*printFunc)(const type *); \
    }
#define DynamicArraySliceStruct(type)    \
    struct {                             \
        const type *data;                \
        const DA_Int count;              \
        void (*printFunc)(const type *); \
    }

#define DefineDynamicArray(name, type) \
    typedef DynamicArrayStruct(type) name

#define DefineDynamicArrayAndSlice(name, type) \
    typedef DynamicArrayStruct(type) name;     \
    typedef DynamicArraySliceStruct(type) name##Slice

#ifndef DYNAMIC_ARRAY_DEFAULT_SIZE
    #define DYNAMIC_ARRAY_DEFAULT_SIZE (256)
#endif // DYNAMIC_ARRAY_DEFAULT_SIZE

#ifndef DYNAMIC_ARRAY_ASSERT
    #include <assert.h>
    #define DYNAMIC_ARRAY_ASSERT assert
#endif // DYNAMIC_ARRAY_ASSERT

#define DynamicArrayReserve(da, newsize)                                            \
    do {                                                                            \
        DA_Int __dar_newSize = (newsize);                                           \
        if ((da)->capacity < __dar_newSize) {                                       \
            if ((da)->capacity == 0) {                                              \
                (da)->capacity = DYNAMIC_ARRAY_DEFAULT_SIZE;                        \
            }                                                                       \
            while ((da)->capacity < __dar_newSize) {                                \
                (da)->capacity *= 2;                                                \
            }                                                                       \
            (da)->data = realloc((da)->data, (da)->capacity * sizeof(*(da)->data)); \
            DYNAMIC_ARRAY_ASSERT((da)->data != nullptr && "Buy more RAM!!!");       \
        }                                                                           \
    } while (0)

#define DynamicArrayAppend(da, x)                   \
    do {                                            \
        DynamicArrayReserve((da), (da)->count + 1); \
        (da)->data[(da)->count++] = (x);            \
    } while (0)

#define DynamicArrayRemoveAt(da, i)                                                                      \
    do {                                                                                                 \
        if ((da)->data) {                                                                                \
            if ((i) >= 0 && (i) < (da)->count) {                                                         \
                for (DA_Int __dara_shiftIdx = i; __dara_shiftIdx < (da)->count - 1; __dara_shiftIdx++) { \
                    (da)->data[__dara_shiftIdx] = (da)->data[__dara_shiftIdx + 1];                       \
                }                                                                                        \
                (da)->count--;                                                                           \
            }                                                                                            \
        }                                                                                                \
    } while (0)

#define DynamicArrayIsEmpty(da) ((da)->count == 0)

#define DynamicArrayLast(da) (DYNAMIC_ARRAY_ASSERT(!DynamicArrayIsEmpty((da))), (da)->data[(da)->count - 1])

#define DynamicArrayAt(da, idx) (DYNAMIC_ARRAY_ASSERT(0 <= (idx) && (idx) < (da)->count), (da)->data[(idx)])

#define DynamicArrayPrint(da)                                                  \
    do {                                                                       \
        DYNAMIC_ARRAY_ASSERT("No print function provided" && (da)->printFunc); \
        printf("[");                                                           \
        for (DA_Int __dap_i = 0; __dap_i < (da)->count; __dap_i++) {           \
            (da)->printFunc(&(da)->data[__dap_i]);                             \
            if (__dap_i < (da)->count - 1) {                                   \
                printf(", ");                                                  \
            }                                                                  \
        }                                                                      \
        printf("]");                                                           \
    } while (0)

#define DynamicArrayPrintN(da)   \
    do {                         \
        DynamicArrayPrint((da)); \
        printf("\n");            \
    } while (0)

#define DynamicArrayClear(da) \
    do {                      \
        (da)->count = 0;      \
    } while (0)

#define DynamicArrayDestroy(da)         \
    do {                                \
        free((da)->data);               \
        memset((da), 0, sizeof(*(da))); \
    } while (0)

#define DynamicArrayForeach(type, var, da) for (type *var = (da)->data; var < (da)->data + (da)->count; var++)

// inclusive indices
#define DynamicArraySlice(da, start_idx, end_idx) \
    {                                             \
        .data = (da)->data + (start_idx),         \
        .count = (end_idx) - (start_idx) + 1,     \
        .printFunc = (da)->printFunc,             \
    };                                            \
    DYNAMIC_ARRAY_ASSERT("Slice out of bounds" && 0 <= (start_idx) && (start_idx) <= (end_idx) && (end_idx) < (da)->count);

#endif // _DYNAMIC_ARRAY_H