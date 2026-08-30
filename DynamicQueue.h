#ifndef _DYNAMIC_QUEUE_H
#define _DYNAMIC_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DynamicQueue(type)               \
    struct {                             \
        type *data;                      \
        int count;                       \
        int capacity;                    \
        void (*printFunc)(const type *); \
    }

#define DefineDynamicQueue(name, type) typedef DynamicQueue(type) name

#ifndef DYNAMIC_QUEUE_DEFAULT_SIZE
#    define DYNAMIC_QUEUE_DEFAULT_SIZE (256)
#endif // DYNAMIC_QUEUE_DEFAULT_SIZE

#ifndef DYNAMIC_QUEUE_ASSERT
#    include <assert.h>
#    define DYNAMIC_QUEUE_ASSERT assert
#endif // DYNAMIC_QUEUE_ASSERT

#ifdef DYNAMIC_QUEUE_IMPLEMENTATION

#    define DynamicQueueReserve(ds, size)                                               \
        do {                                                                            \
            int __dqr_reqSize = (size);                                                 \
            if ((ds)->capacity < __dqr_reqSize) {                                       \
                if ((ds)->capacity == 0) {                                              \
                    (ds)->capacity = DYNAMIC_QUEUE_DEFAULT_SIZE;                        \
                }                                                                       \
                while ((ds)->capacity < __dqr_reqSize) {                                \
                    (ds)->capacity *= 2;                                                \
                }                                                                       \
                (ds)->data = realloc((ds)->data, (ds)->capacity * sizeof(*(ds)->data)); \
                assert((ds)->data != NULL && "Buy more RAM!!!");                        \
            }                                                                           \
        } while (0)

#    define DynamicQueueEnqueue(dq, x)                  \
        do {                                            \
            DynamicQueueReserve((dq), (dq)->count + 1); \
            (dq)->data[(dq)->count++] = (x);            \
        } while (0)

#    define DynamicQueuePeek(dq) (DYNAMIC_QUEUE_ASSERT("Peek into empty queue" && (dq)->count > 0), (dq)->data[0])

// TODO: find a way to compress
#    define DynamicQueueDequeue(dq)                                                           \
        (DYNAMIC_QUEUE_ASSERT("Dequeue from empty queue" && (dq)->count > 0), (dq)->data[0]); \
        do {                                                                                  \
            memmove((dq)->data, (dq)->data + 1, --(dq)->count * sizeof(*(dq)->data));         \
        } while (0)

#    define DynamicQueueIsEmpty(dq) ((bool) ((dq)->count == 0))

#    define DynamicQueuePrint(dq)                                                  \
        do {                                                                       \
            DYNAMIC_QUEUE_ASSERT("No print function provided" && (dq)->printFunc); \
            printf("[");                                                           \
            for (int __dqp_i = 0; __dqp_i < (dq)->count; __dqp_i++) {              \
                (dq)->printFunc(&(dq)->data[__dqp_i]);                             \
                if (__dqp_i < (dq)->count - 1) {                                   \
                    printf(", ");                                                  \
                }                                                                  \
            }                                                                      \
            printf("]");                                                           \
        } while (0)

#    define DynamicQueuePrintN(dq)   \
        do {                         \
            DynamicQueuePrint((dq)); \
            printf("\n");            \
        } while (0)

#    define DynamicQueueClear(dq) \
        do {                      \
            (dq)->count = 0;      \
        } while (0)

#    define DynamicQueueDestroy(dq)             \
        do {                                    \
            if ((dq)->data) {                   \
                free((dq)->data);               \
                memset((dq), 0, sizeof(*(dq))); \
            }                                   \
        } while (0)

#endif // DYNAMIC_QUEUE_IMPLEMENTATION

#endif // _DYNAMIC_QUEUE_H