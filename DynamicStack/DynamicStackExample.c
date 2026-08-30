#include <stdio.h>

#define DYNAMIC_STACK_IMPLEMENTATION
#include "DynamicStack.h"

typedef struct {
    int id;
    int age;
    const char *name;
} Person;

DefineDynamicStack(PersonStack, Person);

// your print function REALLY SHOULD handle null pointers
void printPerson(const Person *p) {
    p ? printf("{name: %s, age: %d, id: %d}", p->name, p->age, p->id) : printf("(NULL)");
}

void example1() {
    PersonStack people = {.printFunc = printPerson};

    DynamicStackPush(&people, ((Person){.name = "John", .age = 23, .id = 123}));

    DynamicStackPush(&people, ((Person){.name = "Stefan", .age = 45, .id = 456}));

    DynamicStackPush(&people, ((Person){.name = "Dana", .age = 90, .id = 789}));

    DynamicStackPrintN(&people);

    Person top;
    while (!DynamicStackIsEmpty(&people)) {
        top = DynamicStackPop(&people);
        printPerson(&top);
        printf("\n");
    }

    DynamicStackDestroy(&people);
}

DefineDynamicStack(CharStack, char);

// check if the parenthesis are valid
bool example2(const char *parenthesis) {
    CharStack pStack = {0};

    for (size_t i = 0; i < strlen(parenthesis); i++) {
        if (parenthesis[i] == '(' || parenthesis[i] == '[' || parenthesis[i] == '{') {
            DynamicStackPush(&pStack, parenthesis[i]);
        } else if (parenthesis[i] == ')' || parenthesis[i] == ']' || parenthesis[i] == '}') {
            char last = DynamicStackPop(&pStack);
            switch (parenthesis[i]) {
                case ')':
                    if (last != '(')
                        return false;
                    break;
                case ']':
                    if (last != '[')
                        return false;
                    break;
                case '}':
                    if (last != '{')
                        return false;
                    break;
            }
        }
    }
    return DynamicStackIsEmpty(&pStack);
}

char *test_strings[] = {
    "()[]{}",   // True
    "([{}])",   // True
    "(]",       // False
    "((()))",   // True
    "(()",      // False
    "{[()()]}", // True
};

int main() {
    printf("\n----- PersonStack Example -----\n");
    example1();
    printf("\n----- PersonStack Example -----\n");

    printf("\n----- Valid Parenthesis Example -----\n");
    for (size_t i = 0; i < sizeof(test_strings) / sizeof(*test_strings); i++) {
        printf("%s", example2(test_strings[i]) ? "True\n" : "False\n");
    }
    printf("\n----- Valid Parenthesis Example -----\n");
    return 0;
}