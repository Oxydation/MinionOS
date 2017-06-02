#include "deviceDriverFs.h"
#include "fileOperations.h"
#include "uartDriver.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEVICES_FOLDER "/dev/"
#define MAX_DEVICES (10)
#define MAX_DEVICE_NAME (10)

typedef struct {
    char name[MAX_DEVICE_NAME];
    FileOperations_t* fileOperations;
} Device_t;

Device_t devices[MAX_DEVICES];
unsigned int deviceCount;

int devFs_open(const char* fileName) {
    if (strncmp(fileName, DEVICES_FOLDER, strlen(DEVICES_FOLDER)) == 0) {
        int i;
        for (i = 0; i < deviceCount; ++i) {
            Device_t device = devices[i];
            const char* searchedName = fileName + strlen(DEVICES_FOLDER);
            if (strcmp(device.name, searchedName) == 0) {
                device.fileOperations->open();
                return i;
            }
        }
    }
    return FILE_NOT_FOUND;
}

void devFs_close(int fileDescriptor) {
    devices[fileDescriptor].fileOperations->release();
}

void devFs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize) {
    devices[fileDescriptor].fileOperations->read(buffer, bufferSize);
}

void devFs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize) {
    devices[fileDescriptor].fileOperations->write(buffer, bufferSize);
}

void devFs_addDevice(const char* name, FileOperations_t* fileOperations) {
    Device_t dev;
    dev.fileOperations = fileOperations;
    strcpy(dev.name, name);
    devices[deviceCount++] = dev;
}

const char* devFs_readdir(const char* dirName) {
    static const char* currentDir;
    static int currentDev;
    if (!currentDir || currentDir != dirName) {
        currentDir = dirName;
        currentDev = 0;
    }

    bool matches = true;
    int i = 0;
    while (dirName[i] && matches) {
        if (dirName[i] != devices[currentDev].name[i]) {
            matches = false;
        } else {
            ++i;
        }
    }
    // provided dirName ends without /
    if (matches && devices[currentDev].name[i] == '/') {
        return devices[currentDev].name[i + 1];
    } else if (matches && devices[currentDev].name[i - 1] == '/') { // end with /
        return devices[currentDev].name[i];
    }

}


void devFs_init() {
    devFs_addDevice("uart1", &devUart1);
    devFs_addDevice("uart2", &devUart2);
    devFs_addDevice("uart3", &devUart3);
}

FileSystem_t deviceDriverFs = {
        .close = devFs_close,
        .open = devFs_open,
        .read = devFs_read,
        .write = devFs_write,
        .readdir = devFs_readdir,
        .init = devFs_init
};
