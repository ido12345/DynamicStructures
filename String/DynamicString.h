#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifndef DYNAMIC_STRING_INT
    #define DYNAMIC_STRING_INT int32_t
#endif // DYNAMIC_STRING_INT
typedef DYNAMIC_STRING_INT DS_Int;

typedef struct {
    char *data;
    DS_Int count; // count == strlen(data)
    DS_Int capacity;
} DynamicString;

void DynamicStringRemoveAll(DynamicString *ds, char x);
bool DynamicStringReadFile(DynamicString *ds, const char *filePath);
bool DynamicStringWriteFile(const char *filePath, DynamicString *ds);

#ifndef DYNAMIC_STRING_DEFAULT_SIZE
    #define DYNAMIC_STRING_DEFAULT_SIZE (256)
#endif // DYNAMIC_STRING_DEFAULT_SIZE

#ifndef DYNAMIC_STRING_ASSERT
    #include <assert.h>
    #define DYNAMIC_STRING_ASSERT assert
#endif

#define DS_Fmt "%.*s"
#define DS_Arg(ds) (ds)->count, (ds)->data

#define DynamicStringReserve(ds, size)                                     \
    do {                                                                   \
        DS_Int __dsr_reqSize = (size);                                     \
        if ((ds)->capacity < __dsr_reqSize) {                              \
            if ((ds)->capacity == 0) {                                     \
                (ds)->capacity = DYNAMIC_STRING_DEFAULT_SIZE;              \
            }                                                              \
            while ((ds)->capacity < __dsr_reqSize) {                       \
                (ds)->capacity *= 2;                                       \
            }                                                              \
            (ds)->data = realloc((ds)->data, (ds)->capacity);              \
            DYNAMIC_STRING_ASSERT(((ds)->data != NULL) && "REALLOC FAIL"); \
        }                                                                  \
    } while (0)

#define DynamicStringAppendf(ds, fstr, ...)                                                    \
    do {                                                                                       \
        DS_Int __dsa_amount = snprintf(NULL, 0, fstr, ##__VA_ARGS__);                          \
        DynamicStringReserve((ds), (ds)->count + __dsa_amount + 1);                            \
        snprintf((ds)->data + (ds)->count, (ds)->capacity - (ds)->count, fstr, ##__VA_ARGS__); \
        (ds)->count += __dsa_amount;                                                           \
        (ds)->data[(ds)->count] = '\0';                                                        \
    } while (0)

#define DynamicStringAppendStr(ds, str, len)                 \
    do {                                                     \
        DynamicStringReserve((ds), (ds)->count + (len) + 1); \
        memcpy((ds)->data + (ds)->count, (str), (len));      \
        (ds)->count += (len);                                \
        (ds)->data[(ds)->count] = '\0';                      \
    } while (0)

#define DynamicStringAppendChar(ds, char)            \
    do {                                             \
        DynamicStringReserve((ds), (ds)->count + 2); \
        (ds)->data[(ds)->count] = (char);            \
        (ds)->count++;                               \
        (ds)->data[(ds)->count] = '\0';              \
    } while (0)

#define DynamicStringAt(ds, idx) (DYNAMIC_STRING_ASSERT(0 <= (idx) && (idx) < (ds)->count), (ds)->data[(idx)])

#define DynamicStringClear(ds) \
    do {                       \
        (ds)->count = 0;       \
        (ds)->data[0] = '\0';  \
    } while (0)

#define DynamicStringPrint(ds)                        \
    do {                                              \
        printf("%.*s", (int)(ds)->count, (ds)->data); \
    } while (0)

// print + newline
#define DynamicStringPrintN(ds)                         \
    do {                                                \
        printf("%.*s\n", (int)(ds)->count, (ds)->data); \
    } while (0)

#define DynamicStringDestroy(ds)        \
    do {                                \
        free((ds)->data);               \
        memset((ds), 0, sizeof(*(ds))); \
    } while (0)

bool DynamicStringReadFile(DynamicString *ds, const char *filePath) {
    bool ret = true;
    FILE *inputFile = NULL;

    inputFile = fopen(filePath, "rb");
    if (!inputFile) {
        fprintf(stderr, "Could not fopen \"%s\": %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    if (fseek(inputFile, 0, SEEK_END) != 0) {
        fprintf(stderr, "Could not fseek \"%s\" to end: %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    DS_Int fileLen = ftell(inputFile);
    if (fileLen < 0) {
        fprintf(stderr, "Could not ftell \"%s\": %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    if (fseek(inputFile, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Could not fseek \"%s\" to start: %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    DynamicStringReserve(ds, fileLen);
    if (fread(ds->data, 1, fileLen, inputFile) != (size_t)fileLen) {
        fprintf(stderr, "Could not fread \"%s\" to dynamic string: %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    ds->count += fileLen;

finish:
    if (inputFile) {
        fclose(inputFile);
    }
    return ret;
}

bool DynamicStringWriteFile(const char *filePath, DynamicString *ds) {
    bool ret = true;
    FILE *outputFile = NULL;

    outputFile = fopen(filePath, "wb");
    if (!outputFile) {
        fprintf(stderr, "Could not fopen \"%s\": %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }
    if (fwrite(ds->data, 1, ds->count, outputFile) != (size_t)ds->count) {
        fprintf(stderr, "Could not fwrite dynamic string to \"%s\": %s\n", filePath, strerror(errno));
        ret = false;
        goto finish;
    }

finish:
    if (outputFile) {
        fclose(outputFile);
    }
    return ret;
}

void DynamicStringRemoveAll(DynamicString *ds, char x) {
    char *dest = ds->data;
    char *src = ds->data;
    DS_Int count = ds->count;
    while ((DS_Int)(src - ds->data) < count) {
        if (*src == x) {
            src++;
            ds->count--;
        } else {
            *dest++ = *src++;
        }
    }
    if (dest < src) {
        *dest = '\0';
    }
}

#endif // _DYNAMIC_STRING_H