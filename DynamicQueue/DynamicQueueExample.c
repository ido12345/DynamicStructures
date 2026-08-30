#include <stdio.h>

#define DYNAMIC_QUEUE_IMPLEMENTATION
#include "DynamicQueue.h"

typedef struct {
    int id;
    int age;
    const char *name;
} Person;

DefineDynamicQueue(PeopleQueue, Person);

// your print function REALLY SHOULD handle null pointers
void printPerson(const Person *p) {
    if (p)
        printf("{name: %s, age: %d, id: %d}", p->name, p->age, p->id);
    else
        printf("(null)");
}

int main() {
    PeopleQueue waitingLine = {.printFunc = printPerson};

    DynamicQueueEnqueue(&waitingLine, ((Person){.name = "John", .age = 23, .id = 123}));

    DynamicQueueEnqueue(&waitingLine, ((Person){.name = "Stefan", .age = 45, .id = 456}));

    DynamicQueueEnqueue(&waitingLine, ((Person){.name = "Dana", .age = 90, .id = 789}));

    DynamicQueuePrintN(&waitingLine);

    Person next = {0};
    while (!DynamicQueueIsEmpty(&waitingLine)) {
        next = DynamicQueueDequeue(&waitingLine);
        printPerson(&next);
        printf("\n");
    }

    DynamicQueueDestroy(&waitingLine);
}