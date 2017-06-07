#include "confFs.h"
#include <string.h>

int confFs_open(const char* fileName) {
    if (strcmp("/etc/uart/uart3.conf", fileName) == 0) {
        return 0;
    }
    return FILE_NOT_FOUND;
}

void confFs_close(int fileDescriptor) {
}

void confFs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize) {
    if (fileDescriptor == 0) {
        strcpy((char*) buffer, "0 0 3 115200 0");
    }
}

void confFs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize) {
}


const char* confFs_readdir(const char* dirName) {
    static const char* currentDir;
    static int successiveCall;
    if (!currentDir || currentDir != dirName) {
        currentDir = dirName;
        successiveCall = 0;
    }

    const char* result;
    if (strcmp("", dirName) == 0 || strcmp("/", dirName) == 0) {
        result = successiveCall == 0 ? "etc" : NULL;
    } else if (strcmp("/etc", dirName) == 0 || strcmp("/etc/", dirName) == 0) {
        result = successiveCall == 0 ? "uart3.conf" : NULL;
    } else {
        result = NULL;
    }
    successiveCall += 1;
    return result;
}

void confFs_init() {

}

FileSystem_t confFs = {
        .close = confFs_close,
        .open = confFs_open,
        .read = confFs_read,
        .write = confFs_write,
        .readdir = confFs_readdir,
        .init = confFs_init
};
