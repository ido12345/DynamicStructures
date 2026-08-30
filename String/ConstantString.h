#ifndef _CONSTANT_STRING_H
#define _CONSTANT_STRING_H

#include <stdio.h>

typedef struct {
    const char *data;
    int count;
} ConstantString;

#define CS_Fmt "%.*s"
#define CS_Arg(cs) (cs)->count, (cs)->data

#define ConstantStringAssemble(str, len) \
    (ConstantString) {                   \
        .data = (str),                   \
        .count = (len),                  \
    }

void ConstantStringPrint(ConstantString *cs) {
    if (cs->data) {
        printf(CS_Fmt, CS_Arg(cs));
    }
}

void ConstantStringPrintN(ConstantString *cs) {
    if (cs->data) {
        printf(CS_Fmt "\n", CS_Arg(cs));
    }
}

#define cs_is_space(c) ((c) == ' ' || (c) == '\n' || (c) == '\t' || (c) == '\r' || (c) == '\v' || (c) == '\f')

void ConstantStringTrimLeft(ConstantString *cs) {
    while (cs_is_space(*cs->data)) {
        cs->data++;
        cs->count--;
    }
}

void ConstantStringTrimRight(ConstantString *cs) {
    while (cs_is_space(cs->data[cs->count - 1])) {
        cs->count--;
    }
}

ConstantString ConstantStringGetLine(ConstantString *cs) {
    ConstantString ret = {0};
    if (cs->count == 0)
        return ret;
    int i = 0;
    while (i < cs->count && cs->data[i] != '\n') {
        i++;
    }

    if (i < cs->count) {
        cs->data += i + 1;
        cs->count -= i + 1;
    } else {
        ret = ConstantStringAssemble(cs->data, cs->count);
        cs->data += cs->count;
        cs->count = 0;
    }
    return ret;
}

ConstantString ConstantStringChopBy(ConstantString *cs, char x) {
    ConstantString ret = {0};
    int i = 0;
    while (i < cs->count && cs->data[i] != x) {
        i++;
    }

    if (i < cs->count) {
        // remove x from both of them
        ret = ConstantStringAssemble(cs->data, i);
        cs->data += i + 1;
        cs->count -= i + 1;
    }
    return ret;
}

ConstantString ConstantStringChopByMultiple(ConstantString *cs, char xs[], int n) {
    ConstantString ret = {0};
    int i = 0;
    while (i < cs->count) {
        for (int j = 0; j < n; j++) {
            if (cs->data[i] == xs[j]) {
                goto found;
            }
        }
        i++;
    }
found:
    if (i < cs->count) {
        // remove found x from both of them
        ret = ConstantStringAssemble(cs->data, i);
        cs->data += i + 1;
        cs->count -= i + 1;
    }
    return ret;
}

int ConstantStringFindChar(ConstantString *cs, char c) {
    for (int i = 0; i < cs->count; i++) {
        if (cs->data[i] == c) {
            return i;
        }
    }
    return -1;
}

#define ConstantStringForeachLine(var, cs) for (ConstantString var = ConstantStringGetLine((cs)); \
                                                var.data;                                         \
                                                var = ConstantStringGetLine((cs)))

#endif // _CONSTANT_STRING_H