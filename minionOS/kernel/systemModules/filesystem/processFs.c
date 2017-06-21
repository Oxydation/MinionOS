#include "processFs.h"
#include "kernel/systemModules/ipc/ipc.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include <string.h>
#include <stdlib.h>

#define stringStartsWith(str1, str2) (strncmp(str1, str2, strlen(str2)))

#define IPC_FOLDER      "/ipc/"

int processFs_open(const char* fileName) {
    // TODO only allow currently used PIDs
    if (stringStartsWith(fileName, IPC_FOLDER) == 0) {
        return atoi(fileName + strlen(IPC_FOLDER));
    } else {
        return FILE_NOT_FOUND;
    }
}

void processFs_close(int fileDescriptor) {
    // NO OP
}

int processFs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize) {
    PCB_t* currentProcess = scheduler_getCurrentProcess();
    if (currentProcess->processId == fileDescriptor) {
        Message_t* message = ipc_receive();
        if (message) {
            int length = strlen(message->message) + 1;
            int bytesToCopy = length > bufferSize ? bufferSize : length;
            memcpy(buffer, message->message, bytesToCopy);
            return bytesToCopy;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void processFs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize) {
    char message[MAX_MESSAGE_LENGTH] = {};
    int bytesToCopy = (MAX_MESSAGE_LENGTH - 1) > bufferSize ? bufferSize : MAX_MESSAGE_LENGTH - 1;
    memcpy(message, buffer, bytesToCopy);
    ipc_message(fileDescriptor, message);
}

const char* processFs_readdir(const char* dirName) {
    static unsigned int consecutiveCall;
    static char previousDir[100];
    if (strcmp(dirName, previousDir) == 0) {
        consecutiveCall += 1;
    } else {
        strncpy(previousDir, dirName, sizeof(previousDir) - 1);
    }

    if (strcmp(dirName, "/") == 0) {
        if (consecutiveCall == 0) {
            return "ipc";
        }
    } else if (stringStartsWith(dirName, IPC_FOLDER) == 0) {
        // TODO return running processes
    }
    consecutiveCall = 0;
    previousDir[0] = '\0';
    return NULL;
}


void processFs_init() {
    // NO OP
}

FileSystem_t processFs = {
        .close = processFs_close,
        .open = processFs_open,
        .read = processFs_read,
        .write = processFs_write,
        .readdir = processFs_readdir,
        .init = processFs_init
};
