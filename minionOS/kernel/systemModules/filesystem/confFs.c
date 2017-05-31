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

void confFs_init() {

}

FileSystem_t confFs = {
        .close = confFs_close,
        .open = confFs_open,
        .read = confFs_read,
        .write = confFs_write,
        .init = confFs_init
};
