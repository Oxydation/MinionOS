#include "semaphore.h"

#define ATOMIC_START()              (_disable_interrupts())
#define ATOMIC_END(previousState)   (_restore_interrupts(previousState))

#define QUEUE_SUCCESS   0
#define QUEUE_FULL      -1
#define QUEUE_EMPTY     -2

static ProcessId_t dequeue(Semaphore_t* semaphore);
static int enqueue(Semaphore_t* semaphore, ProcessId_t);

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
        ProcessId_t processId= &semaphore->queue.processes[semaphore->queue.head];
        semaphore->queue.head = (semaphore->queue.head + 1) % MAX_QUEUED_PROCESSES;
        return processId;
    } else {
        return QUEUE_EMPTY;
    }
}
