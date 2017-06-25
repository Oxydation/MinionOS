#include "ipc.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"
#include <stddef.h>
#include <string.h>

#define MAX_QUEUED_PROCESSES        (10)

typedef struct {
    Message_t messages[MAX_QUEUED_PROCESSES];
    unsigned int tail;
    unsigned int head;
} MessageQueue_t;

MessageQueue_t messageQueues[MAX_ALLOWED_PROCESSES];

static int enqueue(MessageQueue_t* queue, Message_t message);
static const Message_t* dequeue(MessageQueue_t* queue);

int ipc_message(ProcessId_t receiver, const char* messageString) {
    PCB_t* currentProcess = scheduler_getCurrentProcess();
    MessageQueue_t* queue = &messageQueues[receiver];
    Message_t message = { .sender = currentProcess->processId };
    int messageLength = strlen(messageString);
    if (messageLength < MAX_MESSAGE_LENGTH - 1) {
        strcpy(message.message, messageString);
        return enqueue(queue, message);
    } else {
        return MESSAGE_TOO_LONG;
    }
}

const Message_t* ipc_receive() {
    PCB_t* currentProcess = scheduler_getCurrentProcess();
    MessageQueue_t* queue = &messageQueues[currentProcess->processId];
    return dequeue(queue);
}

// https://stackoverflow.com/questions/215557/how-do-i-implement-a-circular-list-ring-buffer-in-c

static int enqueue(MessageQueue_t* queue, Message_t message) {
    if (queue->tail != ((queue->head - 1 + MAX_QUEUED_PROCESSES) % MAX_QUEUED_PROCESSES)) {
        queue->messages[queue->tail] = message;
        queue->tail = (queue->tail + 1) % MAX_QUEUED_PROCESSES;
        return SUCCESS;
    } else {
        return QUEUE_FULL;
    }
}

static const Message_t* dequeue(MessageQueue_t* queue) {
    if (queue->tail != queue->head) {
        Message_t* message = &queue->messages[queue->head];
        queue->head = (queue->head + 1) % MAX_QUEUED_PROCESSES;
        return message;
    } else {
        return NULL;
    }
}
