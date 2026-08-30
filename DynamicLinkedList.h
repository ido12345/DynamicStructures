#ifndef _DYNAMIC_LINKED_LIST_H
#define _DYNAMIC_LINKED_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NodeName(name) name##Node

#define DynamicLinkedListNode(name, type) \
    struct NodeName(name) {               \
        struct NodeName(name) * next;     \
        type value;                       \
    }

#define DynamicLinkedList(name, type)    \
    struct {                             \
        struct NodeName(name) * head;    \
        struct NodeName(name) * temp;    \
        struct NodeName(name) * tail;    \
        void (*printFunc)(const type *); \
    }

#define DefineDynamicLinkedListNode(name, type) typedef DynamicLinkedListNode(name, type) NodeName(name)

#define DefineDynamicLinkedList(name, type)  \
    DefineDynamicLinkedListNode(name, type); \
    typedef DynamicLinkedList(name, type) name

#ifndef DYNAMIC_LINKED_LIST_MALLOC
#    include <stdlib.h>
#    define DYNAMIC_LINKED_LIST_MALLOC malloc
#endif // DYNAMIC_LINKED_LIST_MALLOC

#ifndef DYNAMIC_LINKED_LIST_ASSERT
#    include <assert.h>
#    define DYNAMIC_LINKED_LIST_ASSERT assert
#endif // DYNAMIC_LINKED_LIST_ASSERT

#ifdef DYNAMIC_LINKED_LIST_IMPLEMENTATION

#    define DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL(p) DYNAMIC_LINKED_LIST_ASSERT("Buy more RAM!!!" && (p) != NULL)

#    define DynamicLinkedListPrepend(dll, x)                                    \
        do {                                                                    \
            if (!(dll)->head) {                                                 \
                (dll)->head = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head)); \
                DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->head);               \
                (dll)->head->value = (x);                                       \
                (dll)->head->next = NULL;                                       \
                (dll)->tail = (dll)->head;                                      \
            } else {                                                            \
                (dll)->temp = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head)); \
                DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->temp);               \
                (dll)->temp->value = (x);                                       \
                (dll)->temp->next = (dll)->head;                                \
                (dll)->head = (dll)->temp;                                      \
            }                                                                   \
        } while (0)

#    define DynamicLinkedListAppend(dll, x)                                           \
        do {                                                                          \
            if (!(dll)->head) {                                                       \
                (dll)->head = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head));       \
                DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->head);                     \
                (dll)->head->value = (x);                                             \
                (dll)->head->next = NULL;                                             \
                (dll)->tail = (dll)->head;                                            \
            } else {                                                                  \
                (dll)->tail->next = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head)); \
                DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->tail->next);               \
                (dll)->tail->next->value = (x);                                       \
                (dll)->tail->next->next = NULL;                                       \
                (dll)->tail = (dll)->tail->next;                                      \
            }                                                                         \
        } while (0)

#    define DynamicLinkedListInsertAt(dll, idx, x)                                      \
        do {                                                                            \
            int __dllia_idx_arg = (idx);                                                \
            if (!(dll)->head) {                                                         \
                if (__dllia_idx_arg == 0) {                                             \
                    (dll)->head = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head));     \
                    DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->head);                   \
                    (dll)->head->next = NULL;                                           \
                    (dll)->head->value = (x);                                           \
                    (dll)->tail = (dll)->head;                                          \
                }                                                                       \
            } else {                                                                    \
                if (__dllia_idx_arg == 0) {                                             \
                    (dll)->temp = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head));     \
                    DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->temp);                   \
                    (dll)->temp->next = (dll)->head;                                    \
                    (dll)->temp->value = (x);                                           \
                    (dll)->head = (dll)->temp;                                          \
                } else {                                                                \
                    struct {                                                            \
                        void *next;                                                     \
                    } *__dllia_last = (void *) (dll)->head;                             \
                    int __dllia_index = 0;                                              \
                    while (__dllia_index < __dllia_idx_arg - 1 && __dllia_last) {       \
                        __dllia_index++;                                                \
                        __dllia_last = __dllia_last->next;                              \
                    }                                                                   \
                    if (__dllia_last) {                                                 \
                        (dll)->temp = DYNAMIC_LINKED_LIST_MALLOC(sizeof(*(dll)->head)); \
                        DYNAMIC_LINKED_LIST_ASSERT_NOT_NULL((dll)->temp);               \
                        (dll)->temp->value = (x);                                       \
                        (dll)->temp->next = __dllia_last->next;                         \
                        __dllia_last->next = (dll)->temp;                               \
                        if ((dll)->temp->next == NULL) {                                \
                            (dll)->tail = (dll)->temp;                                  \
                        }                                                               \
                    }                                                                   \
                }                                                                       \
            }                                                                           \
        } while (0)

#    define DynamicLinkedListRemoveAt(dll, idx)                                  \
        do {                                                                     \
            int __dllra_idx_arg = (idx);                                         \
            if ((dll)->head && __dllra_idx_arg >= 0) {                           \
                if (__dllra_idx_arg == 0) {                                      \
                    (dll)->temp = (dll)->head;                                   \
                    (dll)->head = (dll)->head->next;                             \
                    if ((dll)->head == NULL) {                                   \
                        (dll)->tail = NULL;                                      \
                    }                                                            \
                    free((dll)->temp);                                           \
                    (dll)->temp = NULL;                                          \
                } else {                                                         \
                    (dll)->temp = (dll)->head;                                   \
                    int __dllra_index = 0;                                       \
                    while (__dllra_index < __dllra_idx_arg - 1 && (dll)->temp) { \
                        __dllra_index++;                                         \
                        (dll)->temp = (dll)->temp->next;                         \
                    }                                                            \
                    if ((dll)->temp && (dll)->temp->next) {                      \
                        void *__dllra_temp = (dll)->temp->next;                  \
                        (dll)->temp->next = (dll)->temp->next->next;             \
                        if ((dll)->temp->next == NULL) {                         \
                            (dll)->tail = (dll)->temp;                           \
                        }                                                        \
                        free(__dllra_temp);                                      \
                    }                                                            \
                }                                                                \
            }                                                                    \
        } while (0)

#    define DynamicLinkedListRemoveFirst(dll)     \
        do {                                      \
            if ((dll)->head) {                    \
                void *__dllrf_temp = (dll)->head; \
                (dll)->head = (dll)->head->next;  \
                if ((dll)->head == NULL) {        \
                    (dll)->tail = NULL;           \
                    (dll)->temp = NULL;           \
                }                                 \
                free(__dllrf_temp);               \
            }                                     \
        } while (0)

#    define DynamicLinkedListRemoveLast(dll)                \
        do {                                                \
            if ((dll)->head) {                              \
                if ((dll)->head->next) {                    \
                    (dll)->temp = (dll)->head;              \
                    while ((dll)->temp->next->next) {       \
                        (dll)->temp = (dll)->temp->next;    \
                    }                                       \
                    void *__dllrl_temp = (dll)->temp->next; \
                    (dll)->temp->next = NULL;               \
                    (dll)->tail = (dll)->temp;              \
                    free(__dllrl_temp);                     \
                } else {                                    \
                    void *__dllrl_temp = (dll)->head;       \
                    (dll)->head = NULL;                     \
                    (dll)->tail = NULL;                     \
                    (dll)->temp = NULL;                     \
                    free(__dllrl_temp);                     \
                }                                           \
            }                                               \
        } while (0)

#    define DynamicLinkedListGetFirst(dll) ((dll)->head)

#    define DynamicLinkedListGetLast(dll) ((dll)->tail)

#    define DynamicLinkedListGetAt(dll, idx) ({                  \
        void *__dllga_ret = NULL;                                \
        int __dllga_idx_arg = (idx);                             \
        int __dllga_index = 0;                                   \
        (dll)->temp = (dll)->head;                               \
        while (__dllga_index < __dllga_idx_arg && (dll)->temp) { \
            (dll)->temp = (dll)->temp->next;                     \
            __dllga_index++;                                     \
        }                                                        \
        if (__dllga_idx_arg >= 0 && (dll)->temp) {               \
            __dllga_ret = (dll)->temp;                           \
        }                                                        \
        __dllga_ret;                                             \
    })

#    define DynamicLinkedListCount(dll) ({   \
        int __dllc_ret = 0;                  \
        (dll)->temp = (dll)->head;           \
        while ((dll)->temp) {                \
            __dllc_ret++;                    \
            (dll)->temp = (dll)->temp->next; \
        }                                    \
        __dllc_ret;                          \
    })

#    define DynamicLinkedListIsEmpty(dll) ((bool) ((dll)->head == NULL))

#    define DynamicLinkedListForeach(type, var, dll) for (type *var = (dll)->head; var; var = var->next)

#    define DynamicLinkedListPrint(dll)                                                   \
        do {                                                                              \
            DYNAMIC_LINKED_LIST_ASSERT("No print function provided" && (dll)->printFunc); \
            (dll)->temp = (dll)->head;                                                    \
            while ((dll)->temp) {                                                         \
                printf("[");                                                              \
                (dll)->printFunc(&(dll)->temp->value);                                    \
                printf("]");                                                              \
                if ((dll)->temp->next) {                                                  \
                    printf("->");                                                         \
                }                                                                         \
                (dll)->temp = (dll)->temp->next;                                          \
            }                                                                             \
        } while (0)

#    define DynamicLinkedListPrintN(dll)   \
        do {                               \
            DynamicLinkedListPrint((dll)); \
            printf("\n");                  \
        } while (0)

#    define DynamicLinkedListClear(dll)          \
        do {                                     \
            (dll)->temp = (dll)->head;           \
            while ((dll)->temp) {                \
                void *__dllc_prev = (dll)->temp; \
                (dll)->temp = (dll)->temp->next; \
                free(__dllc_prev);               \
            }                                    \
            (dll)->head = NULL;                  \
            (dll)->tail = NULL;                  \
            (dll)->temp = NULL;                  \
        } while (0)

#    define DynamicLinkedListDestroy(dll)        \
        do {                                     \
            (dll)->temp = (dll)->head;           \
            while ((dll)->temp) {                \
                void *__dlld_prev = (dll)->temp; \
                (dll)->temp = (dll)->temp->next; \
                free(__dlld_prev);               \
            }                                    \
            memset((dll), 0, sizeof(*(dll)));    \
        } while (0)

#endif // DYNAMIC_LINKED_LIST_IMPLEMENTATION

#endif // _DYNAMIC_LINKED_LIST_H