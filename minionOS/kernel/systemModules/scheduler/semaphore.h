#ifndef KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_SEMAPHORE_H_
#define KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_SEMAPHORE_H_

#include "scheduler.h"

#define MAX_QUEUED_PROCESSES    MAX_ALLOWED_PROCESSES

typedef struct ProcessQueue ProcessQueue_t;

typedef struct {
    int counter;
    struct {
        ProcessId_t processes[MAX_QUEUED_PROCESSES];
        unsigned int head;
        unsigned int tail;
    } queue;
} Semaphore_t;

Semaphore_t* semaphore_acquire(const char* identifier, int maxConcurrentAccess);

void semaphore_init(Semaphore_t* semaphore, int maxConcurrentAccess);

void semaphore_P(Semaphore_t* semaphore);

void semaphore_V(Semaphore_t* semaphore);

#endif /* KERNEL_SYSTEMMODULES_PROCESSMANAGEMENT_SEMAPHORE_H_ */
