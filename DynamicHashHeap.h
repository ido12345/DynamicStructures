#ifndef _DYNAMIC_HASHHEAP_H
#define _DYNAMIC_HASHHEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum : uint8_t {
    DHH_KV_EMPTY = 0b0000,
    DHH_KV_TAKEN = 0b0001, // taken through open addressing
    DHH_KV_DELETED = 0b0010,
    DHH_KV_DIRTY = 0b0100,
} DHH_KeyValueState;

#ifndef DYNAMIC_HASHHEAP_INT
    #define DYNAMIC_HASHHEAP_INT int32_t
#endif // DYNAMIC_HASHHEAP_INT
#ifndef DYNAMIC_HASHHEAP_UINT
    #define DYNAMIC_HASHHEAP_UINT uint32_t
#endif // DYNAMIC_HASHHEAP_UINT
typedef DYNAMIC_HASHHEAP_INT DHH_Int;
typedef DYNAMIC_HASHHEAP_UINT DHH_UInt;

#define DynamicHashHeapItemName(name) name##KeyValue

#define DynamicHashHeapItem(name, keyType, valueType) \
    struct {                                          \
        keyType key;                                  \
        valueType value;                              \
        DHH_KeyValueState state;                      \
        DHH_Int heapIndex;                            \
    }

#define DynamicHashHeap(name, keyType, valueType)                                                         \
    struct {                                                                                              \
        DynamicHashHeapItemName(name) * data;                                                             \
        DHH_Int count;                                                                                    \
        DHH_Int capacity;                                                                                 \
        DHH_Int *heapData;                                                                                \
        DHH_Int heapCount;                                                                                \
        DHH_Int heapCapacity;                                                                             \
        DHH_UInt (*hashFunc)(const keyType *);                                                            \
        int (*cmpKeyFunc)(const keyType *, const keyType *);                                              \
        void (*printFunc)(const DynamicHashHeapItemName(name) *);                                         \
        int (*cmpHeapFunc)(const DynamicHashHeapItemName(name) *, const DynamicHashHeapItemName(name) *); \
    }

#define DefineDynamicHashHeap(name, keyType, valueType)                                  \
    typedef DynamicHashHeapItem(name, keyType, valueType) DynamicHashHeapItemName(name); \
    typedef DynamicHashHeap(name, keyType, valueType) name

#ifndef DYNAMIC_HASHHEAP_DEFAULT_SIZE
    #define DYNAMIC_HASHHEAP_DEFAULT_SIZE (256)
#endif // DYNAMIC_HASHHEAP_DEFAULT_SIZE

#ifndef DYNAMIC_HASHHEAP_ASSERT
    #include <assert.h>
    #define DYNAMIC_HASHHEAP_ASSERT assert
#endif // DYNAMIC_HASHHEAP_ASSERT

#define DYNAMIC_HASHHEAP_ASSERT_NOT_NULL(p) (DYNAMIC_HASHHEAP_ASSERT((p) != nullptr && "Buy more RAM!!!"))

#define DYNAMIC_HASHHEAP_HASH_ASSERT(dhh) (DYNAMIC_HASHHEAP_ASSERT((dhh)->hashFunc != nullptr && "Dynamic HashHeap is missing a hash function"))

#define DYNAMIC_HASHHEAP_CMP_ASSERT(dhh) (DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc != nullptr && "Dynamic HashHeap is missing a compare function"))

#define DYNAMIC_HASHHEAP_CMP_HEAP_ASSERT(dhh) (DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpHeapFunc != nullptr && "Dynamic HashHeap is missing a heap compare function"))

#define DYNAMIC_HASHHEAP_PRINT_ASSERT(dhh) (DYNAMIC_HASHHEAP_ASSERT((dhh)->printFunc != nullptr && "Dynamic HashHeap is missing a print function"))

#define DYNAMIC_HASHHEAP_VAR_EMPTY_INIT(x) x = ((typeof(x)){})

#define DYNAMIC_HASHHEAP_IS_EMPTY(dhh) ((dhh)->data == nullptr || (dhh)->count == 0 || (dhh)->capacity == 0)

#define DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh) ((dhh)->heapData == nullptr || (dhh)->heapCount == 0 || (dhh)->heapCapacity == 0)

#define DynamicHashHeapParent(i) (((i) - 1) / 2)

#define DynamicHashHeapLeft(i) (((i) * 2) + 1)

#define DynamicHashHeapRight(i) (((i) * 2) + 2)

#define DynamicHashHeapGenericSwap(a, b) \
    do {                                 \
        typeof(a) __gs_temp = (a);       \
        (a) = (b);                       \
        (b) = __gs_temp;                 \
    } while (0)

#define DynamicHashHeapReserve(dhh, size)                                                                            \
    do {                                                                                                             \
        DHH_Int __dhhr_size = (size);                                                                                \
        if ((dhh)->capacity < __dhhr_size) {                                                                         \
            DHH_Int __dhhr_newCapacity = (dhh)->capacity;                                                            \
            if (__dhhr_newCapacity == 0) {                                                                           \
                __dhhr_newCapacity = DYNAMIC_HASHHEAP_DEFAULT_SIZE;                                                  \
            }                                                                                                        \
            while (__dhhr_newCapacity < __dhhr_size) {                                                               \
                __dhhr_newCapacity *= 2;                                                                             \
            }                                                                                                        \
            (dhh)->data = realloc((dhh)->data, __dhhr_newCapacity * sizeof(*(dhh)->data));                           \
            DYNAMIC_HASHHEAP_ASSERT_NOT_NULL((dhh)->data);                                                           \
            memset((dhh)->data + (dhh)->capacity, 0, (__dhhr_newCapacity - (dhh)->capacity) * sizeof(*(dhh)->data)); \
            for (DHH_Int __dhhr_i = 0; __dhhr_i < (dhh)->capacity; __dhhr_i++) {                                     \
                if ((dhh)->data[__dhhr_i].state == DHH_KV_TAKEN) {                                                   \
                    (dhh)->data[__dhhr_i].state = DHH_KV_DIRTY;                                                      \
                } else {                                                                                             \
                    (dhh)->data[__dhhr_i].state = DHH_KV_EMPTY;                                                      \
                }                                                                                                    \
            }                                                                                                        \
            (dhh)->heapCount = 0;                                                                                    \
            for (DHH_Int __dhhr_i = 0; __dhhr_i < (dhh)->capacity; __dhhr_i++) {                                     \
                typeof((dhh)->data[0]) __dhhr_cur = (dhh)->data[__dhhr_i];                                           \
                if (__dhhr_cur.state != DHH_KV_DIRTY) continue;                                                      \
                (dhh)->data[__dhhr_i].state = DHH_KV_EMPTY;                                                          \
                while (__dhhr_cur.state != DHH_KV_EMPTY) {                                                           \
                    DHH_UInt __dhhr_hashIdx = (dhh)->hashFunc(&__dhhr_cur.key) % __dhhr_newCapacity;                 \
                    while ((dhh)->data[__dhhr_hashIdx].state == DHH_KV_TAKEN) {                                      \
                        __dhhr_hashIdx = (__dhhr_hashIdx + 1) % __dhhr_newCapacity;                                  \
                    }                                                                                                \
                    __dhhr_cur.state = DHH_KV_TAKEN;                                                                 \
                    __dhhr_cur.heapIndex = (dhh)->heapCount;                                                         \
                    DynamicHashHeapGenericSwap((dhh)->data[__dhhr_hashIdx], __dhhr_cur);                             \
                    DynamicHashHeapInsertHeap((dhh), __dhhr_hashIdx);                                                \
                }                                                                                                    \
            }                                                                                                        \
            (dhh)->capacity = __dhhr_newCapacity;                                                                    \
        }                                                                                                            \
    } while (0)

#define DynamicHashHeapHeapifyUp(dhh, i)                                                                                                      \
    do {                                                                                                                                      \
        DYNAMIC_HASHHEAP_CMP_HEAP_ASSERT(dhh);                                                                                                \
        if (!DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh)) {                                                                                           \
            DHH_Int __dhhhu_cur = (i);                                                                                                        \
            if (0 <= __dhhhu_cur && __dhhhu_cur < (dhh)->heapCount) {                                                                         \
                while (__dhhhu_cur > 0) {                                                                                                     \
                    DHH_Int __dhhhu_parent = DynamicHashHeapParent(__dhhhu_cur);                                                              \
                    if ((dhh)->cmpHeapFunc(&(dhh)->data[(dhh)->heapData[__dhhhu_cur]], &(dhh)->data[(dhh)->heapData[__dhhhu_parent]]) >= 0) { \
                        break;                                                                                                                \
                    }                                                                                                                         \
                    DynamicHashHeapGenericSwap((dhh)->heapData[__dhhhu_cur], (dhh)->heapData[__dhhhu_parent]);                                \
                    DynamicHashHeapGenericSwap(__dhhhu_cur, __dhhhu_parent);                                                                  \
                    (dhh)->data[(dhh)->heapData[__dhhhu_cur]].heapIndex = __dhhhu_cur;                                                        \
                    (dhh)->data[(dhh)->heapData[__dhhhu_parent]].heapIndex = __dhhhu_parent;                                                  \
                }                                                                                                                             \
            }                                                                                                                                 \
        }                                                                                                                                     \
    } while (0)

#define DynamicHashHeapHeapifyDown(dhh, i)                                                                                                                                          \
    do {                                                                                                                                                                            \
        DYNAMIC_HASHHEAP_CMP_HEAP_ASSERT(dhh);                                                                                                                                      \
        if (!DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh)) {                                                                                                                                 \
            DHH_Int __dhhhd_cur = (i);                                                                                                                                              \
            if (0 <= __dhhhd_cur && __dhhhd_cur < (dhh)->heapCount) {                                                                                                               \
                while (true) {                                                                                                                                                      \
                    DHH_Int __dhhhd_left = DynamicHashHeapLeft(__dhhhd_cur);                                                                                                        \
                    DHH_Int __dhhhd_right = DynamicHashHeapRight(__dhhhd_cur);                                                                                                      \
                    DHH_Int __dhhhd_min_max = __dhhhd_cur;                                                                                                                          \
                    if (__dhhhd_left < (dhh)->heapCount && (dhh)->cmpHeapFunc(&(dhh)->data[(dhh)->heapData[__dhhhd_left]], &(dhh)->data[(dhh)->heapData[__dhhhd_min_max]]) < 0) {   \
                        __dhhhd_min_max = __dhhhd_left;                                                                                                                             \
                    }                                                                                                                                                               \
                    if (__dhhhd_right < (dhh)->heapCount && (dhh)->cmpHeapFunc(&(dhh)->data[(dhh)->heapData[__dhhhd_right]], &(dhh)->data[(dhh)->heapData[__dhhhd_min_max]]) < 0) { \
                        __dhhhd_min_max = __dhhhd_right;                                                                                                                            \
                    }                                                                                                                                                               \
                    if (__dhhhd_cur == __dhhhd_min_max) {                                                                                                                           \
                        break;                                                                                                                                                      \
                    }                                                                                                                                                               \
                    DynamicHashHeapGenericSwap((dhh)->heapData[__dhhhd_cur], (dhh)->heapData[__dhhhd_min_max]);                                                                     \
                    DynamicHashHeapGenericSwap(__dhhhd_cur, __dhhhd_min_max);                                                                                                       \
                    (dhh)->data[(dhh)->heapData[__dhhhd_cur]].heapIndex = __dhhhd_cur;                                                                                              \
                    (dhh)->data[(dhh)->heapData[__dhhhd_min_max]].heapIndex = __dhhhd_min_max;                                                                                      \
                }                                                                                                                                                                   \
            }                                                                                                                                                                       \
        }                                                                                                                                                                           \
    } while (0)

#define DynamicHashHeapReserveHeap(dhh, size)                                                           \
    do {                                                                                                \
        DHH_Int __dhhrh_size = (size);                                                                  \
        if ((dhh)->heapCapacity < __dhhrh_size) {                                                       \
            if ((dhh)->heapCapacity == 0) {                                                             \
                (dhh)->heapCapacity = DYNAMIC_ARRAY_DEFAULT_SIZE;                                       \
            }                                                                                           \
            while ((dhh)->heapCapacity < __dhhrh_size) {                                                \
                (dhh)->heapCapacity *= 2;                                                               \
            }                                                                                           \
            (dhh)->heapData = realloc((dhh)->heapData, (dhh)->heapCapacity * sizeof(*(dhh)->heapData)); \
            DYNAMIC_HASHHEAP_ASSERT_NOT_NULL((dhh)->heapData);                                          \
        }                                                                                               \
    } while (0)

#define DynamicHashHeapInsertHeap(dhh, i)                        \
    do {                                                         \
        DynamicHashHeapReserveHeap((dhh), (dhh)->heapCount + 1); \
        DHH_Int __dhhih_heapIndex = (dhh)->heapCount;            \
        (dhh)->heapCount++;                                      \
        (dhh)->heapData[__dhhih_heapIndex] = (i);                \
        DynamicHashHeapHeapifyUp((dhh), __dhhih_heapIndex);      \
    } while (0)

#define DynamicHashHeapRemoveHeap(dhh, i)                                                                    \
    do {                                                                                                     \
        if (!DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh)) {                                                          \
            DHH_Int __dhhrh_cur = (i);                                                                       \
            if (0 <= __dhhrh_cur && __dhhrh_cur < (dhh)->heapCount) {                                        \
                (dhh)->heapCount--;                                                                          \
                DHH_Int __dhhrh_last = (dhh)->heapCount;                                                     \
                if (__dhhrh_cur != __dhhrh_last) {                                                           \
                    DynamicHashHeapGenericSwap((dhh)->heapData[__dhhrh_cur], (dhh)->heapData[__dhhrh_last]); \
                    (dhh)->data[(dhh)->heapData[__dhhrh_last]].heapIndex = -1;                               \
                    (dhh)->data[(dhh)->heapData[__dhhrh_cur]].heapIndex = __dhhrh_cur;                       \
                    DynamicHashHeapHeapifyDown((dhh), __dhhrh_cur);                                          \
                    DynamicHashHeapHeapifyUp((dhh), __dhhrh_cur);                                            \
                } else {                                                                                     \
                    (dhh)->data[(dhh)->heapData[__dhhrh_cur]].heapIndex = -1;                                \
                }                                                                                            \
            }                                                                                                \
        }                                                                                                    \
    } while (0)

#define DYNAMIC_HASHHEAP_INSERT_NOGROW(dhh, k, v)                                                                                     \
    do {                                                                                                                              \
        DHH_UInt __dhhi_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                                                            \
        while ((dhh)->data[__dhhi_hashIdx].state == DHH_KV_TAKEN && (dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhi_hashIdx].key) != 0) { \
            __dhhi_hashIdx = (__dhhi_hashIdx + 1) % (dhh)->capacity;                                                                  \
        }                                                                                                                             \
        (dhh)->data[__dhhi_hashIdx].value = (v);                                                                                      \
        if ((dhh)->data[__dhhi_hashIdx].state != DHH_KV_TAKEN) {                                                                      \
            (dhh)->count++;                                                                                                           \
            (dhh)->data[__dhhi_hashIdx].state = DHH_KV_TAKEN;                                                                         \
            (dhh)->data[__dhhi_hashIdx].key = (k);                                                                                    \
            DHH_Int __dhhi_heapIdx = (dhh)->heapCount;                                                                                \
            (dhh)->data[__dhhi_hashIdx].heapIndex = __dhhi_heapIdx;                                                                   \
            DynamicHashHeapInsertHeap((dhh), __dhhi_hashIdx);                                                                         \
        }                                                                                                                             \
    } while (0)

#define DynamicHashHeapInsert(dhh, k, v)                                    \
    do {                                                                    \
        DYNAMIC_HASHHEAP_HASH_ASSERT(dhh);                                  \
        DYNAMIC_HASHHEAP_CMP_ASSERT(dhh);                                   \
        DynamicHashHeapReserve((dhh), (dhh)->count + (dhh)->count / 4 + 1); \
        DYNAMIC_HASHHEAP_INSERT_NOGROW((dhh), (k), (v));                    \
    } while (0)

#define DYNAMIC_HASHHEAP_FIND_KEY_INDEX(dhh, k, i)                                                                                                          \
    while (((dhh)->data[(i)].state == DHH_KV_DELETED) || ((dhh)->data[(i)].state != DHH_KV_EMPTY && (dhh)->cmpKeyFunc(&(k), &(dhh)->data[(i)].key) != 0)) { \
        i = ((i) + 1) % (dhh)->capacity;                                                                                                                    \
    }

#define DynamicHashHeapRemove(dhh, k)                                                                     \
    do {                                                                                                  \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                            \
            DYNAMIC_HASHHEAP_HASH_ASSERT(dhh);                                                            \
            DYNAMIC_HASHHEAP_CMP_ASSERT(dhh);                                                             \
            DHH_UInt __dhhrm_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhrm_hashIdx);                                 \
            if ((dhh)->data[__dhhrm_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhrm_hashIdx].key) == 0); \
                (dhh)->count--;                                                                           \
                DYNAMIC_HASHHEAP_VAR_EMPTY_INIT((dhh)->data[__dhhrm_hashIdx].key);                        \
                DYNAMIC_HASHHEAP_VAR_EMPTY_INIT((dhh)->data[__dhhrm_hashIdx].value);                      \
                (dhh)->data[__dhhrm_hashIdx].state = DHH_KV_DELETED;                                      \
                DynamicHashHeapRemoveHeap((dhh), (dhh)->data[__dhhrm_hashIdx].heapIndex);                 \
            }                                                                                             \
        }                                                                                                 \
    } while (0)

#define DynamicHashHeapGetHead(dhh, ret)             \
    do {                                             \
        typeof((dhh)->heapData[0]) __dhheh_ret = {}; \
        if (!DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh)) {  \
            __dhheh_ret = (dhh)->heapData[0];        \
        }                                            \
        ret = __dhheh_ret;                           \
    } while (0)

#define DynamicHashHeapExtractHead(dhh, ret)                                                   \
    do {                                                                                       \
        typeof((dhh)->heapData[0]) __dhheh_ret = {};                                           \
        if (!DYNAMIC_HASHHEAP_HEAP_IS_EMPTY(dhh)) {                                            \
            (dhh)->heapCount--;                                                                \
            __dhheh_ret = (dhh)->heapData[0];                                                  \
            DynamicHashHeapGenericSwap((dhh)->heapData[0], (dhh)->heapData[(dhh)->heapCount]); \
            DynamicHashHeapHeapifyDown((dhh), 0);                                              \
        }                                                                                      \
        ret = __dhheh_ret;                                                                     \
    } while (0)

#define DynamicHashHeapGetIndex(dhh, k, ret)                                                              \
    do {                                                                                                  \
        DHH_Int __dhhgi_ret = -1;                                                                         \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                            \
            DHH_UInt __dhhgi_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhgi_hashIdx);                                 \
            if ((dhh)->data[__dhhgi_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhgi_hashIdx].key) == 0); \
                __dhhgi_ret = __dhhgi_hashIdx;                                                            \
            }                                                                                             \
        }                                                                                                 \
        ret = __dhhgi_ret;                                                                                \
    } while (0)

#define DynamicHashHeapGetValue(dhh, k, ret)                                                              \
    do {                                                                                                  \
        typeof((dhh)->data[0].value) __dhhgv_ret = {};                                                    \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                            \
            DHH_UInt __dhhgv_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhgv_hashIdx);                                 \
            if ((dhh)->data[__dhhgv_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhgv_hashIdx].key) == 0); \
                __dhhgv_ret = (dhh)->data[__dhhgv_hashIdx].value;                                         \
            }                                                                                             \
        }                                                                                                 \
        ret = __dhhgv_ret;                                                                                \
    } while (0)

#define DynamicHashHeapGetValuePtr(dhh, k, ret)                                                            \
    do {                                                                                                   \
        typeof((dhh)->data[0].value) *__dhhgvp_ret = {};                                                   \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                             \
            DHH_UInt __dhhgvp_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhgvp_hashIdx);                                 \
            if ((dhh)->data[__dhhgvp_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhgvp_hashIdx].key) == 0); \
                __dhhgvp_ret = &(dhh)->data[__dhhgvp_hashIdx].value;                                       \
            }                                                                                              \
        }                                                                                                  \
        ret = __dhhgvp_ret;                                                                                \
    } while (0)

#define DynamicHashHeapGetKeyValue(dhh, k, ret)                                                            \
    do {                                                                                                   \
        typeof((dhh)->data[0]) __dhhgkv_ret = {};                                                          \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                             \
            DHH_UInt __dhhgkv_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhgkv_hashIdx);                                 \
            if ((dhh)->data[__dhhgkv_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhgkv_hashIdx].key) == 0); \
                __dhhgkv_ret = (dhh)->data[__dhhgkv_hashIdx];                                              \
            }                                                                                              \
        }                                                                                                  \
        ret = __dhhgkv_ret;                                                                                \
    } while (0)

#define DynamicHashHeapGetKeyValuePtr(dhh, k, ret)                                                         \
    do {                                                                                                   \
        typeof((dhh)->data[0]) *__dhhgvp_ret = {};                                                         \
        if (!DYNAMIC_HASHHEAP_IS_EMPTY(dhh)) {                                                             \
            DHH_UInt __dhhgvp_hashIdx = (dhh)->hashFunc(&(k)) % (dhh)->capacity;                           \
            DYNAMIC_HASHHEAP_FIND_KEY_INDEX((dhh), (k), __dhhgvp_hashIdx);                                 \
            if ((dhh)->data[__dhhgvp_hashIdx].state == DHH_KV_TAKEN) {                                     \
                DYNAMIC_HASHHEAP_ASSERT((dhh)->cmpKeyFunc(&(k), &(dhh)->data[__dhhgvp_hashIdx].key) == 0); \
                __dhhgvp_ret = &(dhh)->data[__dhhgvp_hashIdx];                                             \
            }                                                                                              \
        }                                                                                                  \
        ret = __dhhgvp_ret;                                                                                \
    } while (0)

#define DynamicHashHeapForeach(var, dhh)                                \
    for (DHH_Int __dhhf_i = 0; __dhhf_i < (dhh)->heapCount; __dhhf_i++) \
        for (DHH_Int __dhhf_j = 1; (var = (dhh)->data[(dhh)->heapData[__dhhf_i]], __dhhf_j); __dhhf_j--)

#define DynamicHashHeapForeachPtr(var, dhh)                             \
    for (DHH_Int __dhhf_i = 0; __dhhf_i < (dhh)->heapCount; __dhhf_i++) \
        for (DHH_Int __dhhf_j = 1; (var = &(dhh)->data[(dhh)->heapData[__dhhf_i]], __dhhf_j); __dhhf_j--)

#define DynamicHashHeapPrint(dhh)                                             \
    do {                                                                      \
        printf("[");                                                          \
        DYNAMIC_HASHHEAP_PRINT_ASSERT(dhh);                                   \
        for (DHH_Int __dhhp_i = 0; __dhhp_i < (dhh)->heapCount; __dhhp_i++) { \
            (dhh)->printFunc(&(dhh)->data[(dhh)->heapData[__dhhp_i]]);        \
            if (__dhhp_i < (dhh)->heapCount - 1) {                            \
                printf(", ");                                                 \
            }                                                                 \
        }                                                                     \
        printf("]");                                                          \
    } while (0)

#define DynamicHashHeapPrintN(dhh) \
    do {                           \
        DynamicHashHeapPrint(dhh); \
        printf("\n");              \
    } while (0)

#define DynamicHashHeapClear(dhh)                                                                        \
    do {                                                                                                 \
        if ((dhh)->data) memset((dhh)->data, 0, (dhh)->capacity * sizeof(*(dhh)->data));                 \
        if ((dhh)->heapData) memset((dhh)->heapData, 0, (dhh)->heapCapacity * sizeof(*(dhh)->heapData)); \
        (dhh)->count = 0;                                                                                \
        (dhh)->heapCount = 0;                                                                            \
    } while (0)

#define DynamicHashHeapDestroy(dhh)       \
    do {                                  \
        free((dhh)->data);                \
        free((dhh)->heapData);            \
        memset((dhh), 0, sizeof(*(dhh))); \
    } while (0)

#endif // _DYNAMIC_HASHHEAP_H
