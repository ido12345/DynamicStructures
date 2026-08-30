#ifndef _DYNAMIC_STACK_H
#define _DYNAMIC_STACK_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DynamicStack(type)               \
    struct {                             \
        type *data;                      \
        int count;                       \
        int capacity;                    \
        void (*printFunc)(const type *); \
    }

#define DefineDynamicStack(name, type) typedef DynamicStack(type) name

#ifndef DYNAMIC_STACK_DEFAULT_SIZE
    #define DYNAMIC_STACK_DEFAULT_SIZE (256)
#endif // DYNAMIC_STACK_DEFAULT_SIZE

#ifndef DYNAMIC_STACK_ASSERT
    #include <assert.h>
    #define DYNAMIC_STACK_ASSERT assert
#endif // DYNAMIC_STACK_ASSERT

#define DynamicStackReserve(ds, size)                                               \
    do {                                                                            \
        int __dsr_reqSize = (size);                                                 \
        if ((ds)->capacity < __dsr_reqSize) {                                       \
            if ((ds)->capacity == 0) {                                              \
                (ds)->capacity = DYNAMIC_STACK_DEFAULT_SIZE;                        \
            }                                                                       \
            while ((ds)->capacity < __dsr_reqSize) {                                \
                (ds)->capacity *= 2;                                                \
            }                                                                       \
            (ds)->data = realloc((ds)->data, (ds)->capacity * sizeof(*(ds)->data)); \
            DYNAMIC_STACK_ASSERT((ds)->data != NULL && "Buy more RAM!!!");          \
        }                                                                           \
    } while (0)

#define DynamicStackPush(ds, x)                     \
    do {                                            \
        DynamicStackReserve((ds), (ds)->count + 1); \
        (ds)->data[(ds)->count++] = x;              \
    } while (0)

#define DynamicStackPeek(ds) (DYNAMIC_STACK_ASSERT("Peek into empty stack" && (ds)->count > 0), (ds)->data[(ds)->count - 1])

#define DynamicStackPop(ds) (DYNAMIC_STACK_ASSERT("Peek from empty stack" && (ds)->count > 0), (ds)->data[--(ds)->count])

#define DynamicStackIsEmpty(ds) ((bool)((ds)->count == 0))

#define DynamicStackPrint(ds)                                                  \
    do {                                                                       \
        DYNAMIC_STACK_ASSERT("No print function provided" && (ds)->printFunc); \
        printf("[");                                                           \
        for (int __dsp_i = 0; __dsp_i < (ds)->count; __dsp_i++) {              \
            (ds)->printFunc(&(ds)->data[__dsp_i]);                             \
            if (__dsp_i < (ds)->count - 1) {                                   \
                printf(", ");                                                  \
            }                                                                  \
        }                                                                      \
        printf("]");                                                           \
    } while (0)

#define DynamicStackPrintN(ds)   \
    do {                         \
        DynamicStackPrint((ds)); \
        printf("\n");            \
    } while (0)

#define DynamicStackClear(ds) \
    do {                      \
        (ds)->count = 0;      \
    } while (0)

#define DynamicStackDestroy(ds)             \
    do {                                    \
        if ((ds)->data) {                   \
            free((ds)->data);               \
            memset((ds), 0, sizeof(*(ds))); \
        }                                   \
    } while (0)

#endif // _DYNAMIC_STACK_H