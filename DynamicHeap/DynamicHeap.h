#ifndef _DYNAMIC_HEAP_H
#define _DYNAMIC_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef DYNAMIC_HEAP_INT
    #define DYNAMIC_HEAP_INT int32_t
#endif // DYNAMIC_HEAP_INT
typedef DYNAMIC_HEAP_INT DH_Int;

#define DynamicHeap(type)           \
    struct {                        \
        type *data;                 \
        DH_Int count;               \
        DH_Int capacity;            \
        int (*cmpFunc)(type, type); \
        void (*printFunc)(type);    \
    }

#define DefineDynamicHeap(name, type) typedef DynamicHeap(type) name;

#define DYNAMIC_HEAP_DEFAULT_SIZE (256)

#ifndef DYNAMIC_HEAP_ASSERT
    #include <assert.h>
    #define DYNAMIC_HEAP_ASSERT assert
#endif // DYNAMIC_HEAP_ASSERT

#define DynamicHeapReserve(dh, size)                                                \
    do {                                                                            \
        DH_Int __dhr_reqSize = (size);                                              \
        if ((dh)->capacity < __dhr_reqSize) {                                       \
            if ((dh)->capacity == 0) {                                              \
                (dh)->capacity = DYNAMIC_HEAP_DEFAULT_SIZE;                         \
            }                                                                       \
            while ((dh)->capacity < __dhr_reqSize) {                                \
                (dh)->capacity *= 2;                                                \
            }                                                                       \
            (dh)->data = realloc((dh)->data, (dh)->capacity * sizeof(*(dh)->data)); \
            DYNAMIC_HEAP_ASSERT((dh)->data != nullptr && "Buy more RAM!!!");        \
        }                                                                           \
    } while (0)

#define HEAP_PARENT(i) (((i) - 1) / 2)

#define HEAP_LEFT(i) (((i) * 2) + 1)

#define HEAP_RIGHT(i) (((i) * 2) + 2)

#define DYNAMIC_HEAP_GENERIC_SWAP(a, b) \
    do {                                \
        typeof(a) __gs_temp = (a);      \
        (a) = (b);                      \
        (b) = __gs_temp;                \
    } while (0)

#define DYNAMIC_HEAP_CMP_ASSERT(dh) DYNAMIC_HEAP_ASSERT("Dynamic Heap " #dh " is missing a compare function" && (dh)->cmpFunc != nullptr)
#define DYNAMIC_HEAP_PRINT_ASSERT(dh) DYNAMIC_HEAP_ASSERT("Dynamic Heap " #dh " is missing a print function" && (dh)->printFunc != nullptr);

#define DynamicHeapHeapifyUp(dh, i)                                                           \
    do {                                                                                      \
        DYNAMIC_HEAP_CMP_ASSERT(dh);                                                          \
        if (((dh)->data && (dh)->count >= 0) && ((i) >= 0 && (i) < (dh)->count)) {            \
            DH_Int __dhhu_cur = (i);                                                          \
            while ((__dhhu_cur > 0)) {                                                        \
                DH_Int __dhhu_parent = HEAP_PARENT(__dhhu_cur);                               \
                if ((dh)->cmpFunc((dh)->data[__dhhu_cur], (dh)->data[__dhhu_parent]) >= 0) {  \
                    break;                                                                    \
                }                                                                             \
                DYNAMIC_HEAP_GENERIC_SWAP((dh)->data[__dhhu_cur], (dh)->data[__dhhu_parent]); \
                __dhhu_cur = __dhhu_parent;                                                   \
            }                                                                                 \
        }                                                                                     \
    } while (0)

#define DynamicHeapHeapifyDown(dh, i)                                                                                        \
    do {                                                                                                                     \
        DYNAMIC_HEAP_CMP_ASSERT(dh);                                                                                         \
        if (((dh)->data && (dh)->count >= 0) && ((i) >= 0 && (i) < (dh)->count)) {                                           \
            DH_Int __dhhd_cur = (i);                                                                                         \
            while (true) {                                                                                                   \
                DH_Int __dhhd_left = HEAP_LEFT(__dhhd_cur);                                                                  \
                DH_Int __dhhd_right = HEAP_RIGHT(__dhhd_cur);                                                                \
                DH_Int __dhhd_min_max = __dhhd_cur;                                                                          \
                if (__dhhd_left < (dh)->count && (dh)->cmpFunc((dh)->data[__dhhd_left], (dh)->data[__dhhd_min_max]) < 0) {   \
                    __dhhd_min_max = __dhhd_left;                                                                            \
                }                                                                                                            \
                if (__dhhd_right < (dh)->count && (dh)->cmpFunc((dh)->data[__dhhd_right], (dh)->data[__dhhd_min_max]) < 0) { \
                    __dhhd_min_max = __dhhd_right;                                                                           \
                }                                                                                                            \
                if (__dhhd_cur == __dhhd_min_max) {                                                                          \
                    break;                                                                                                   \
                }                                                                                                            \
                DYNAMIC_HEAP_GENERIC_SWAP((dh)->data[__dhhd_cur], (dh)->data[__dhhd_min_max]);                               \
                __dhhd_cur = __dhhd_min_max;                                                                                 \
            }                                                                                                                \
        }                                                                                                                    \
    } while (0)

#define DynamicHeapInsert(dh, x)                     \
    do {                                             \
        DynamicHeapReserve((dh), (dh)->count + 1);   \
        (dh)->data[(dh)->count] = (x);               \
        (dh)->count++;                               \
        DynamicHeapHeapifyUp((dh), (dh)->count - 1); \
    } while (0)

#define DynamicHeapIsEmpty(dh) ((dh)->count > 0)

#define DynamicHeapIndexOf(dh, x, ret)                                   \
    do {                                                                 \
        DH_Int __dhf_ret = -1;                                           \
        if (!DynamicHeapIsEmpty(dh)) {                                   \
            for (DH_Int __dhf_i = 0; __dhf_i < (dh)->count; __dhf_i++) { \
                if ((dh)->cmpFunc((dh)->data[__dhf_i], (x)) == 0) {      \
                    __dhf_ret = __dhf_i;                                 \
                    break;                                               \
                }                                                        \
            }                                                            \
        }                                                                \
        ret = __dhf_ret;                                                 \
    } while (0)

#define DynamicHeapRemove(dh, x)                                                               \
    do {                                                                                       \
        if ((dh)->data) {                                                                      \
            DH_Int __dhr_cur = 0;                                                              \
            DynamicHeapIndexOf((dh), (x), __dhr_cur);                                          \
            if (__dhr_cur > -1) {                                                              \
                DYNAMIC_HEAP_GENERIC_SWAP((dh)->data[__dhr_cur], (dh)->data[(dh)->count - 1]); \
                (dh)->count--;                                                                 \
                DynamicHeapHeapifyDown((dh), __dhr_cur);                                       \
                DynamicHeapHeapifyUp((dh), __dhr_cur);                                         \
            }                                                                                  \
        }                                                                                      \
    } while (0)

#define DynamicHeapGetHead(dh) (DYNAMIC_HEAP_ASSERT("Empty Heap" && (dh)->count > 0), (dh)->data[0])

#define DynamicHeapExtractHead(dh)                                                 \
    DynamicHeapGetHead((dh));                                                      \
    do {                                                                           \
        if ((dh)->data && (dh)->count > 0) {                                       \
            DYNAMIC_HEAP_GENERIC_SWAP((dh)->data[0], (dh)->data[(dh)->count - 1]); \
            (dh)->count--;                                                         \
            DynamicHeapHeapifyDown((dh), 0);                                       \
        }                                                                          \
    } while (0)

#define DynamicHeapPrint(dh)                                         \
    do {                                                             \
        DYNAMIC_HEAP_PRINT_ASSERT(dh);                               \
        printf("[");                                                 \
        for (DH_Int __dhp_i = 0; __dhp_i < (dh)->count; __dhp_i++) { \
            (dh)->printFunc((dh)->data[__dhp_i]);                    \
            if (__dhp_i < (dh)->count - 1) {                         \
                printf(", ");                                        \
            }                                                        \
        }                                                            \
        printf("]");                                                 \
    } while (0)

#define DynamicHeapPrintN(dh)   \
    do {                        \
        DynamicHeapPrint((dh)); \
        printf("\n");           \
    } while (0)

#define DynamicHeapClear(dh) \
    do {                     \
        (dh)->count = 0;     \
    } while (0)

#define DynamicHeapDestroy(dh)          \
    do {                                \
        free((dh)->data);               \
        memset((dh), 0, sizeof(*(dh))); \
    } while (0)

#endif // _DYNAMIC_HEAP_H