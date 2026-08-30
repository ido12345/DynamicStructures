#include <stdio.h>

#define DYNAMIC_HASHMAP_DEFAULT_SIZE (2)
#define DYNAMIC_HASHMAP_IMPLEMENTATION
#include "DynamicHashmap.h"

DefineDynamicHashmap(StringFreq, const char *, int);

void printStringFreqKeyValue(const StringFreqKeyValue *sfkv) {
    if (sfkv && sfkv->key) {
        printf("(\"%s\": %d)", sfkv->key, sfkv->value);
    } else {
        printf("(null)");
    }
}

uint32_t stringHash(const char *str) {
    int strLen = strlen(str);
    uint32_t hash = 5381;
    for (int i = 0; i < strLen; i++) {
        hash *= 33;
        hash += str[i];
    }
    return hash;
}

int main() {
    StringFreq stringsFreq = {.hashFunc = stringHash, .cmpKeyFunc = strcmp, .printFunc = printStringFreqKeyValue};

    DynamicHashmapInsert(&stringsFreq, "Abc", 1);
    DynamicHashmapPrintN(&stringsFreq);
#ifdef DEBUG
    for (StringFreqKeyValue *i = stringsFreq.data; i < stringsFreq.data + stringsFreq.capacity; i++) {
        printStringFreqKeyValue(i);
    }
    printf("\n");
#endif

    printf("--------------------------------------------------\n");

    DynamicHashmapInsert(&stringsFreq, "ABc", 2);
#ifdef DEBUG
    for (StringFreqKeyValue *i = stringsFreq.data; i < stringsFreq.data + stringsFreq.capacity; i++) {
        printStringFreqKeyValue(i);
    }
    printf("\n");
#endif
    DynamicHashmapPrintN(&stringsFreq);

    printf("--------------------------------------------------\n");

    printf("{");
    StringFreqKeyValue sfkv;
    DynamicHashmapForeach(sfkv, &stringsFreq) {
        printf("[\"%s\": %d]", sfkv.key, sfkv.value);
    }
    printf("}\n");

    printf("--------------------------------------------------\n");

#ifdef DEBUG
    for (StringFreqKeyValue *i = stringsFreq.data; i < stringsFreq.data + stringsFreq.capacity; i++) {
        printStringFreqKeyValue(i);
    }
    printf("\n");
#endif

    DynamicHashmapRemove(&stringsFreq, "ABc");
    DynamicHashmapPrintN(&stringsFreq);

#ifdef DEBUG
    for (StringFreqKeyValue *i = stringsFreq.data; i < stringsFreq.data + stringsFreq.capacity; i++) {
        printStringFreqKeyValue(i);
    }
    printf("\n");
#endif

    printf("--------------------------------------------------\n");

    int idxOfAbc = {};
    DynamicHashmapGetIndex(&stringsFreq, "Abc", idxOfAbc);
    StringFreqKeyValue AbcKV1 = (idxOfAbc > -1) ? stringsFreq.data[idxOfAbc] : (StringFreqKeyValue){};
    printStringFreqKeyValue(&AbcKV1);
    printf("\n");

    // does exist, gets a shallow copy
    StringFreqKeyValue AbcKV2 = {};
    DynamicHashmapGetKeyValue(&stringsFreq, "Abc", AbcKV2);
    printStringFreqKeyValue(&AbcKV2);
    printf("\n");

    // doesnt exist, this returns an empty struct (values = {}),
    // and the KV state is KV_EMPTY so you know its not just values that are 0, its actually empty values
    StringFreqKeyValue AbcKV3 = {};
    DynamicHashmapGetKeyValue(&stringsFreq, "ABc", AbcKV3);
    if (DHM_IS_KV_TAKEN(AbcKV3)) {
        printStringFreqKeyValue(&AbcKV3);
        printf("\n");
    }

    StringFreqKeyValue *AbcKV4 = 0;
    DynamicHashmapGetKeyValuePtr(&stringsFreq, "Abc", AbcKV4);
    AbcKV4->value = 10;
    // since this is a pointer the actual KV's value got updated in the hashmap

    // also this is dangerous because you could change the key and lose the... lemme just show you

    const char *saved = AbcKV4->key;
    AbcKV4->key = nullptr;

    DynamicHashmapPrintN(&stringsFreq);

    // will ⚠️ segfault ⚠️ use with caution ⚠️
#if false
    DynamicHashmapGetKeyValue(&stringsFreq, "Abc", AbcKV2);
    if (DHM_IS_KV_TAKEN(AbcKV2)) {
        printStringFreqKeyValue(&AbcKV2);
        printf("\n");
    }
#endif
    // this segfaults because strcmp gets null from the key when comparing, and it doesnt handle null

    AbcKV4->key = saved;

    DynamicHashmapPrintN(&stringsFreq);

    printf("--------------------------------------------------\n");

    // example of the hashmap not saving the order when removing

    DynamicHashmapInsert(&stringsFreq, "Hello", 3);
    DynamicHashmapInsert(&stringsFreq, "World", 6);
    DynamicHashmapInsert(&stringsFreq, "Potatoes", 1);

    StringFreqKeyValue stringFreqKV;
    printf("[");
    DynamicHashmapForeach(stringFreqKV, &stringsFreq) {
        printStringFreqKeyValue(&stringFreqKV);
    }
    printf("]\n");

    DynamicHashmapRemove(&stringsFreq, "Hello");
    printf("[");
    DynamicHashmapForeach(stringFreqKV, &stringsFreq) {
        printStringFreqKeyValue(&stringFreqKV);
    }
    printf("]\n");

    printf("--------------------------------------------------\n");

    // an example of iterating over copies of the struct VS iterating the pointers
    // when you modify the pointers' values it will change the actual hashmap's values

    printf("[");
    StringFreqKeyValue stringFreqKVstruct;
    DynamicHashmapForeach(stringFreqKVstruct, &stringsFreq) {
        stringFreqKVstruct.value++;
        printStringFreqKeyValue(&stringFreqKVstruct);
    }
    printf("]\n");

    DynamicHashmapPrintN(&stringsFreq);

    printf("\n");

    printf("[");
    StringFreqKeyValue *stringFreqKVptr;
    DynamicHashmapForeachPtr(stringFreqKVptr, &stringsFreq) {
        stringFreqKVptr->value++;
        printStringFreqKeyValue(stringFreqKVptr);
    }
    printf("]\n");

    DynamicHashmapPrintN(&stringsFreq);

    printf("--------------------------------------------------\n");

    DynamicHashmapClear(&stringsFreq);
    DynamicHashmapPrintN(&stringsFreq);

    printf("--------------------------------------------------\n");

    DynamicHashmapDestroy(&stringsFreq);

    return 0;
}