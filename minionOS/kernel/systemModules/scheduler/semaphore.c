#include "semaphore.h"
#include <string.h>

#define MAX_MANAGED_SEMAPHORES      16
#define MAX_IDENTIFIER              100

#define ATOMIC_START()              (_disable_interrupts())
#define ATOMIC_END(previousState)   (_restore_interrupts(previousState))

#define QUEUE_SUCCESS   0
#define QUEUE_FULL      -1
#define QUEUE_EMPTY     MAX_ALLOWED_PROCESSES + 1

typedef struct {
    char identifier[MAX_IDENTIFIER + 1];
    Semaphore_t sem;
} ManagedSemaphore_t;

static ManagedSemaphore_t managedSemaphores[MAX_MANAGED_SEMAPHORES];

static ProcessId_t dequeue(Semaphore_t* semaphore);
static int enqueue(Semaphore_t* semaphore, ProcessId_t);

Semaphore_t* semaphore_acquire(const char* identifier) {
    if (strlen(identifier) > MAX_IDENTIFIER) {
        return NULL;
    }
    int i;
    int firstFree = -1;
    // find previously acquired semaphore
    for (i = 0; i < MAX_MANAGED_SEMAPHORES; i++) {
        if (managedSemaphores[i].identifier[0] == '\0' && firstFree == -1) {
            firstFree = i;
        } else if (strcmp(managedSemaphores->identifier, identifier) == 0) {
            return &managedSemaphores[i].sem;
        }
    }
    // allocate new semaphore
    if (firstFree != -1) {
        ManagedSemaphore_t* managedSemaphore = &managedSemaphores[firstFree];
        strcpy(managedSemaphore->identifier, identifier);
        return &managedSemaphore->sem;
    } else {
        return NULL;
    }
}

void semaphore_init(Semaphore_t* semaphore, int maxConcurrentAccess) {
    semaphore->counter = maxConcurrentAccess;
}

void semaphore_P(Semaphore_t* semaphore) {
    int previousState = ATOMIC_START();
    semaphore->counter--;
    if (semaphore->counter < 0) {
        ProcessId_t currentProcess = scheduler_getCurrentProcess()->processId;
        enqueue(semaphore, currentProcess);
        scheduler_blockProcess(currentProcess);
        scheduler_prepareSwitchToIdleProcess();
    }
    ATOMIC_END(previousState);
}

void semaphore_V(Semaphore_t* semaphore) {
    int previousState = ATOMIC_START();
    semaphore->counter++;
    if (semaphore->counter <= 0) {
        scheduler_unblockProcess(dequeue(semaphore));
    }
    ATOMIC_END(previousState);
}

void semaphore_managedP(const char* identifier) {
    Semaphore_t* semaphore = semaphore_acquire(identifier);
    if (semaphore) {
        semaphore_P(semaphore);
    }
}

void semaphore_managedV(const char* identifier) {
    Semaphore_t* semaphore = semaphore_acquire(identifier);
    if (semaphore) {
        semaphore_V(semaphore);
    }
}

void semaphore_managedInit(const char* identifier, int maxConcurrentAccess) {
    Semaphore_t* semaphore = semaphore_acquire(identifier);
    if (semaphore) {
        semaphore_init(semaphore, maxConcurrentAccess);
    }
}

// https://stackoverflow.com/questions/215557/how-do-i-implement-a-circular-list-ring-buffer-in-c

static int enqueue(Semaphore_t* semaphore, ProcessId_t processId) {
    if (semaphore->queue.tail != ((semaphore->queue.head - 1 + MAX_QUEUED_PROCESSES) % MAX_QUEUED_PROCESSES)) {
        semaphore->queue.processes[semaphore->queue.tail] = processId;
        semaphore->queue.tail = (semaphore->queue.tail + 1) % MAX_QUEUED_PROCESSES;
        return QUEUE_SUCCESS;
    } else {
        return QUEUE_FULL;
    }
}

static ProcessId_t dequeue(Semaphore_t* semaphore) {
    if (semaphore->queue.tail != semaphore->queue.head) {
        ProcessId_t processId = semaphore->queue.processes[semaphore->queue.head];
        semaphore->queue.head = (semaphore->queue.head + 1) % MAX_QUEUED_PROCESSES;
        return processId;
    } else {
        return QUEUE_EMPTY;
    }
}
