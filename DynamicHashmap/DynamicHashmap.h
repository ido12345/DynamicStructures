#ifndef _DYNAMIC_HASHMAP_H
#define _DYNAMIC_HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum : uint8_t {
    DHM_KV_EMPTY = 0b0000,
    DHM_KV_TAKEN = 0b0001, // taken through open addressing
    DHM_KV_TAKEN_HEAD = 0b0010,
    DHM_KV_DELETED = 0b0100,
    DHM_KV_DIRTY = 0b1000,
} DHM_KeyValueState;

#ifndef DYNAMIC_HASHMAP_INT
    #define DYNAMIC_HASHMAP_INT int32_t
#endif // DYNAMIC_HASHMAP_INT
#ifndef DYNAMIC_HASHMAP_UINT
    #define DYNAMIC_HASHMAP_UINT uint32_t
#endif // DYNAMIC_HASHMAP_UINT
typedef DYNAMIC_HASHMAP_INT DHM_Int;
typedef DYNAMIC_HASHMAP_UINT DHM_UInt;

#define DynamicHashmapItemName(name) name##KeyValue

#define DynamicHashmapItem(name, keyType, valueType) \
    struct {                                         \
        keyType key;                                 \
        valueType value;                             \
        DHM_KeyValueState state;                     \
        DHM_Int addressDataIndex;                    \
    }

#define DynamicHashmap(name, keyType, valueType)                 \
    struct {                                                     \
        DynamicHashmapItemName(name) * data;                     \
        DHM_Int count;                                           \
        DHM_Int capacity;                                        \
        DHM_Int *addressData;                                    \
        DHM_Int addressCount;                                    \
        DHM_Int addressCapacity;                                 \
        DHM_UInt (*hashFunc)(keyType);                           \
        DHM_Int (*cmpKeyFunc)(keyType, keyType);                 \
        void (*printFunc)(const DynamicHashmapItemName(name) *); \
    }

#define DefineDynamicHashmap(name, keyType, valueType)                                 \
    typedef DynamicHashmapItem(name, keyType, valueType) DynamicHashmapItemName(name); \
    typedef DynamicHashmap(name, keyType, valueType) name

#ifndef DYNAMIC_HASHMAP_DEFAULT_SIZE
    #define DYNAMIC_HASHMAP_DEFAULT_SIZE (256)
#endif // DYNAMIC_HASHMAP_DEFAULT_SIZE

#ifndef DYNAMIC_HASHMAP_ASSERT
    #include <assert.h>
    #define DYNAMIC_HASHMAP_ASSERT assert
#endif // DYNAMIC_HASHMAP_ASSERT

#define DHM_IS_KV_TAKEN(kv) ((kv).state & (DHM_KV_TAKEN_HEAD | DHM_KV_TAKEN))

#define DYNAMIC_HASHMAP_ASSERT_NOT_NULL(p) (DYNAMIC_HASHMAP_ASSERT((p) != nullptr && "Buy more RAM!!!"))

#define DYNAMIC_HASHMAP_HASH_ASSERT(dhm) (DYNAMIC_HASHMAP_ASSERT((dhm)->hashFunc != nullptr && "Dynamic Hashmap is missing a hash function"))

#define DYNAMIC_HASHMAP_CMP_ASSERT(dhm) (DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc != nullptr && "Dynamic Hashmap is missing a compare function"))

#define DYNAMIC_HASHMAP_PRINT_ASSERT(dhm) (DYNAMIC_HASHMAP_ASSERT((dhm)->printFunc != nullptr && "Dynamic Hashmap is missing a print function"))

#define DYNAMIC_HASHMAP_VAR_EMPTY_INIT(x) x = ((typeof(x)){})

#define DYNAMIC_HASHMAP_IS_EMPTY(dhm) ((dhm)->data == nullptr || (dhm)->count == 0 || (dhm)->capacity == 0)

#define DynamicHashmapGenericSwap(a, b) \
    do {                                \
        typeof(a) __gs_temp = (a);      \
        (a) = (b);                      \
        (b) = __gs_temp;                \
    } while (0)

#define DynamicHashmapReserve(dhm, size)                                                                             \
    do {                                                                                                             \
        DHM_Int __dhmr_size = (size);                                                                                \
        if ((dhm)->capacity < __dhmr_size) {                                                                         \
            DHM_Int __dhmr_newCapacity = (dhm)->capacity;                                                            \
            if (__dhmr_newCapacity == 0) {                                                                           \
                __dhmr_newCapacity = DYNAMIC_HASHMAP_DEFAULT_SIZE;                                                   \
            }                                                                                                        \
            while (__dhmr_newCapacity < __dhmr_size) {                                                               \
                __dhmr_newCapacity *= 2;                                                                             \
            }                                                                                                        \
            (dhm)->data = realloc((dhm)->data, __dhmr_newCapacity * sizeof(*(dhm)->data));                           \
            DYNAMIC_HASHMAP_ASSERT_NOT_NULL((dhm)->data);                                                            \
            memset((dhm)->data + (dhm)->capacity, 0, (__dhmr_newCapacity - (dhm)->capacity) * sizeof(*(dhm)->data)); \
            for (DHM_Int __dhmr_i = 0; __dhmr_i < (dhm)->capacity; __dhmr_i++) {                                     \
                if (DHM_IS_KV_TAKEN((dhm)->data[__dhmr_i])) {                                                        \
                    (dhm)->data[__dhmr_i].state = DHM_KV_DIRTY;                                                      \
                } else {                                                                                             \
                    (dhm)->data[__dhmr_i].state = DHM_KV_EMPTY;                                                      \
                }                                                                                                    \
            }                                                                                                        \
            (dhm)->addressCount = 0;                                                                                 \
            for (DHM_Int __dhmr_i = 0; __dhmr_i < (dhm)->capacity; __dhmr_i++) {                                     \
                typeof((dhm)->data[0]) __dhmr_cur = (dhm)->data[__dhmr_i];                                           \
                if (__dhmr_cur.state != DHM_KV_DIRTY) continue;                                                      \
                (dhm)->data[__dhmr_i].state = DHM_KV_EMPTY;                                                          \
                while (__dhmr_cur.state != DHM_KV_EMPTY) {                                                           \
                    DHM_UInt __dhmr_initialHashIdx = (dhm)->hashFunc(__dhmr_cur.key) % __dhmr_newCapacity;           \
                    DHM_UInt __dhmr_hashIdx = __dhmr_initialHashIdx;                                                 \
                    while (DHM_IS_KV_TAKEN((dhm)->data[__dhmr_hashIdx])) {                                           \
                        __dhmr_hashIdx = (__dhmr_hashIdx + 1) % __dhmr_newCapacity;                                  \
                    }                                                                                                \
                    DynamicHashmapGenericSwap((dhm)->data[__dhmr_hashIdx], __dhmr_cur);                              \
                    if (__dhmr_initialHashIdx == __dhmr_hashIdx) {                                                   \
                        (dhm)->data[__dhmr_hashIdx].state = DHM_KV_TAKEN_HEAD;                                       \
                        DynamicHashmapInsertAddress((dhm), __dhmr_hashIdx);                                          \
                    } else {                                                                                         \
                        (dhm)->data[__dhmr_hashIdx].state = DHM_KV_TAKEN;                                            \
                    }                                                                                                \
                }                                                                                                    \
            }                                                                                                        \
            (dhm)->capacity = __dhmr_newCapacity;                                                                    \
        }                                                                                                            \
    } while (0)

#define DynamicHashmapReserveAddress(dhm, size)                                                                                                   \
    do {                                                                                                                                          \
        DHM_Int __dhmra_size = (size);                                                                                                            \
        if ((dhm)->addressCapacity < __dhmra_size) {                                                                                              \
            DHM_Int __dhmra_old_capacity = (dhm)->addressCapacity;                                                                                \
            if ((dhm)->addressCapacity == 0) {                                                                                                    \
                (dhm)->addressCapacity = DYNAMIC_HASHMAP_DEFAULT_SIZE;                                                                            \
            }                                                                                                                                     \
            while ((dhm)->addressCapacity < __dhmra_size) {                                                                                       \
                (dhm)->addressCapacity *= 2;                                                                                                      \
            }                                                                                                                                     \
            (dhm)->addressData = realloc((dhm)->addressData, (dhm)->addressCapacity * sizeof(*(dhm)->addressData));                               \
            DYNAMIC_HASHMAP_ASSERT_NOT_NULL((dhm)->addressData);                                                                                  \
            memset((dhm)->addressData + __dhmra_old_capacity, -1, ((dhm)->addressCapacity - __dhmra_old_capacity) * sizeof(*(dhm)->addressData)); \
        }                                                                                                                                         \
    } while (0)

#define DynamicHashmapInsertAddress(dhm, idx)                         \
    do {                                                              \
        DynamicHashmapReserveAddress((dhm), (dhm)->addressCount + 1); \
        (dhm)->addressData[(dhm)->addressCount] = (idx);              \
        (dhm)->data[(idx)].addressDataIndex = (dhm)->addressCount;    \
        (dhm)->addressCount++;                                        \
    } while (0);

#define DYNAMIC_HASHMAP_INSERT_NOGROW(dhm, k, v)                                                                               \
    do {                                                                                                                       \
        DHM_UInt __dhmi_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                                                      \
        DHM_UInt __dhmi_initialHashIdx = __dhmi_hashIdx;                                                                       \
        while (DHM_IS_KV_TAKEN((dhm)->data[__dhmi_hashIdx]) && (dhm)->cmpKeyFunc((k), (dhm)->data[__dhmi_hashIdx].key) != 0) { \
            __dhmi_hashIdx = (__dhmi_hashIdx + 1) % (dhm)->capacity;                                                           \
        }                                                                                                                      \
        /* if the kv is a new one initialize some stuff, otherwise update only the value */                                    \
        if (!DHM_IS_KV_TAKEN((dhm)->data[__dhmi_hashIdx])) {                                                                   \
            (dhm)->count++;                                                                                                    \
            if (__dhmi_hashIdx == __dhmi_initialHashIdx) {                                                                     \
                DynamicHashmapInsertAddress(dhm, __dhmi_hashIdx);                                                              \
                (dhm)->data[__dhmi_hashIdx].state = DHM_KV_TAKEN_HEAD;                                                         \
            } else {                                                                                                           \
                (dhm)->data[__dhmi_hashIdx].state = DHM_KV_TAKEN;                                                              \
            }                                                                                                                  \
            (dhm)->data[__dhmi_hashIdx].key = (k);                                                                             \
        }                                                                                                                      \
        (dhm)->data[__dhmi_hashIdx].value = (v);                                                                               \
    } while (0)

#define DynamicHashmapInsert(dhm, k, v)                                    \
    do {                                                                   \
        DYNAMIC_HASHMAP_HASH_ASSERT(dhm);                                  \
        DYNAMIC_HASHMAP_CMP_ASSERT(dhm);                                   \
        DynamicHashmapReserve((dhm), (dhm)->count + (dhm)->count / 3 + 2); \
        DYNAMIC_HASHMAP_INSERT_NOGROW((dhm), (k), (v));                    \
    } while (0)

#define DYNAMIC_HASHMAP_FIND_KEY_INDEX(dhm, k, i)                                                                                                           \
    while ((((dhm)->data[(i)].state == DHM_KV_DELETED) || ((dhm)->data[(i)].state != DHM_KV_EMPTY && (dhm)->cmpKeyFunc((k), (dhm)->data[(i)].key) != 0))) { \
        i = ((i) + 1) % (dhm)->capacity;                                                                                                                    \
    }

#define DynamicHashmapRemove(dhm, k)                                                                                                          \
    do {                                                                                                                                      \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                                                                 \
            DYNAMIC_HASHMAP_HASH_ASSERT(dhm);                                                                                                 \
            DYNAMIC_HASHMAP_CMP_ASSERT(dhm);                                                                                                  \
            DHM_UInt __dhmr_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                                                                 \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmr_hashIdx);                                                                       \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmr_hashIdx])) {                                                                               \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmr_hashIdx].key) == 0);                                         \
                (dhm)->count--;                                                                                                               \
                if ((dhm)->data[__dhmr_hashIdx].state == DHM_KV_TAKEN_HEAD) {                                                                 \
                    DHM_Int __dhmr_nextIdx = (__dhmr_hashIdx + 1) % (dhm)->capacity;                                                          \
                    while ((dhm)->data[__dhmr_nextIdx].state & DHM_KV_DELETED) {                                                              \
                        __dhmr_nextIdx = (__dhmr_nextIdx + 1) % (dhm)->capacity;                                                              \
                    }                                                                                                                         \
                    if ((dhm)->data[__dhmr_nextIdx].state != DHM_KV_TAKEN) {                                                                  \
                        (dhm)->addressCount--;                                                                                                \
                        (dhm)->addressData[(dhm)->data[__dhmr_hashIdx].addressDataIndex] = (dhm)->addressData[(dhm)->addressCount];           \
                        (dhm)->data[(dhm)->addressData[(dhm)->addressCount]].addressDataIndex = (dhm)->data[__dhmr_hashIdx].addressDataIndex; \
                        (dhm)->addressData[(dhm)->addressCount] = -1;                                                                         \
                    } else {                                                                                                                  \
                        (dhm)->data[__dhmr_nextIdx].addressDataIndex = (dhm)->data[__dhmr_hashIdx].addressDataIndex;                          \
                        (dhm)->addressData[(dhm)->data[__dhmr_nextIdx].addressDataIndex] = __dhmr_nextIdx;                                    \
                        (dhm)->data[__dhmr_nextIdx].state = DHM_KV_TAKEN_HEAD;                                                                \
                    }                                                                                                                         \
                }                                                                                                                             \
                DYNAMIC_HASHMAP_VAR_EMPTY_INIT((dhm)->data[__dhmr_hashIdx].key);                                                              \
                DYNAMIC_HASHMAP_VAR_EMPTY_INIT((dhm)->data[__dhmr_hashIdx].value);                                                            \
                (dhm)->data[__dhmr_hashIdx].state = DHM_KV_DELETED;                                                                           \
                (dhm)->data[__dhmr_hashIdx].addressDataIndex = -1;                                                                            \
            }                                                                                                                                 \
        }                                                                                                                                     \
    } while (0)

#define DynamicHashmapGetIndex(dhm, k, ret)                                                            \
    do {                                                                                               \
        DHM_Int __dhmgi_ret = -1;                                                                      \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                          \
            DHM_UInt __dhmgi_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                         \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmgi_hashIdx);                               \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmgi_hashIdx])) {                                       \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmgi_hashIdx].key) == 0); \
                __dhmgi_ret = __dhmgi_hashIdx;                                                         \
            }                                                                                          \
        }                                                                                              \
        ret = __dhmgi_ret;                                                                             \
    } while (0)

#define DynamicHashmapGetValue(dhm, k, ret)                                                            \
    do {                                                                                               \
        typeof((dhm)->data[0].value) __dhmgv_ret = {};                                                 \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                          \
            DHM_UInt __dhmgv_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                         \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmgv_hashIdx);                               \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmgv_hashIdx])) {                                       \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmgv_hashIdx].key) == 0); \
                __dhmgv_ret = (dhm)->data[__dhmgv_hashIdx].value;                                      \
            }                                                                                          \
        }                                                                                              \
        ret = __dhmgv_ret;                                                                             \
    } while (0)

#define DynamicHashmapGetValuePtr(dhm, k, ret)                                                          \
    do {                                                                                                \
        typeof((dhm)->data[0].value) *__dhmgvp_ret = nullptr;                                           \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                           \
            DHM_UInt __dhmgvp_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                         \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmgvp_hashIdx);                               \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmgvp_hashIdx])) {                                       \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmgvp_hashIdx].key) == 0); \
                __dhmgvp_ret = &(dhm)->data[__dhmgvp_hashIdx].value;                                    \
            }                                                                                           \
        }                                                                                               \
        ret = __dhmgvp_ret;                                                                             \
    } while (0)

#define DynamicHashmapGetKeyValue(dhm, k, ret)                                                          \
    do {                                                                                                \
        typeof((dhm)->data[0]) __dhmgkv_ret = {};                                                       \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                           \
            DHM_UInt __dhmgkv_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                         \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmgkv_hashIdx);                               \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmgkv_hashIdx])) {                                       \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmgkv_hashIdx].key) == 0); \
                __dhmgkv_ret = (dhm)->data[__dhmgkv_hashIdx];                                           \
            }                                                                                           \
        }                                                                                               \
        ret = __dhmgkv_ret;                                                                             \
    } while (0)

#define DynamicHashmapGetKeyValuePtr(dhm, k, ret)                                                       \
    do {                                                                                                \
        typeof((dhm)->data[0]) *__dhmgvp_ret = nullptr;                                                 \
        if (!DYNAMIC_HASHMAP_IS_EMPTY(dhm)) {                                                           \
            DHM_UInt __dhmgvp_hashIdx = (dhm)->hashFunc((k)) % (dhm)->capacity;                         \
            DYNAMIC_HASHMAP_FIND_KEY_INDEX((dhm), (k), __dhmgvp_hashIdx);                               \
            if (DHM_IS_KV_TAKEN((dhm)->data[__dhmgvp_hashIdx])) {                                       \
                DYNAMIC_HASHMAP_ASSERT((dhm)->cmpKeyFunc((k), (dhm)->data[__dhmgvp_hashIdx].key) == 0); \
                __dhmgvp_ret = &(dhm)->data[__dhmgvp_hashIdx];                                          \
            }                                                                                           \
        }                                                                                               \
        ret = __dhmgvp_ret;                                                                             \
    } while (0)

#define DynamicHashmapForeach(var, dhm)                                                                                                                                                                                             \
    for (DHM_Int __dhmf_i = 0; __dhmf_i < (dhm)->addressCount; __dhmf_i++)                                                                                                                                                          \
        for (DHM_Int __dhmf_j = 0; (var = (dhm)->data[((dhm)->addressData[__dhmf_i] + __dhmf_j) % (dhm)->capacity]).state & ((__dhmf_j == 0) ? (DHM_KV_TAKEN_HEAD | DHM_KV_DELETED) : (DHM_KV_TAKEN | DHM_KV_DELETED)); __dhmf_j++) \
            if (DHM_IS_KV_TAKEN(var))

#define DynamicHashmapForeachPtr(var, dhm)                                                                                                                                                                                            \
    for (DHM_Int __dhmf_i = 0; __dhmf_i < (dhm)->addressCount; __dhmf_i++)                                                                                                                                                            \
        for (DHM_Int __dhmf_j = 0; (var = &(dhm)->data[((dhm)->addressData[__dhmf_i] + __dhmf_j) % (dhm)->capacity])->state & ((__dhmf_j == 0) ? (DHM_KV_TAKEN_HEAD | DHM_KV_DELETED) : (DHM_KV_TAKEN | DHM_KV_DELETED)); __dhmf_j++) \
            if (DHM_IS_KV_TAKEN(*var))

#define DynamicHashmapPrint(dhm)                                                                                                                                         \
    do {                                                                                                                                                                 \
        printf("[");                                                                                                                                                     \
        DYNAMIC_HASHMAP_PRINT_ASSERT(dhm);                                                                                                                               \
        for (DHM_Int __dhmp_i = 0; __dhmp_i < (dhm)->addressCount; __dhmp_i++) {                                                                                         \
            if ((dhm)->data[(dhm)->addressData[__dhmp_i]].state & DHM_KV_TAKEN_HEAD) (dhm)->printFunc(&(dhm)->data[(dhm)->addressData[__dhmp_i]]);                       \
            for (DHM_Int __dhmp_j = 1; ((dhm)->data[((dhm)->addressData[__dhmp_i] + __dhmp_j) % (dhm)->capacity].state) & (DHM_KV_TAKEN | DHM_KV_DELETED); __dhmp_j++) { \
                if ((dhm)->data[((dhm)->addressData[__dhmp_i] + __dhmp_j) % (dhm)->capacity].state != DHM_KV_DELETED) {                                                  \
                    (dhm)->printFunc(&(dhm)->data[((dhm)->addressData[__dhmp_i] + __dhmp_j) % (dhm)->capacity]);                                                         \
                }                                                                                                                                                        \
            }                                                                                                                                                            \
        }                                                                                                                                                                \
        printf("]");                                                                                                                                                     \
    } while (0)

#define DynamicHashmapPrintN(dhm) \
    do {                          \
        DynamicHashmapPrint(dhm); \
        printf("\n");             \
    } while (0)

#define DynamicHashmapClear(dhm)                                                                                     \
    do {                                                                                                             \
        if ((dhm)->data) memset((dhm)->data, 0, (dhm)->capacity * sizeof(*(dhm)->data));                             \
        if ((dhm)->addressData) memset((dhm)->addressData, 0, (dhm)->addressCapacity * sizeof(*(dhm)->addressData)); \
        (dhm)->count = 0;                                                                                            \
        (dhm)->addressCount = 0;                                                                                     \
    } while (0)

#define DynamicHashmapDestroy(dhm)        \
    do {                                  \
        free((dhm)->data);                \
        free((dhm)->addressData);         \
        memset((dhm), 0, sizeof(*(dhm))); \
    } while (0)

#endif // _DYNAMIC_HASHMAP_H
