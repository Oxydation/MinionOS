#include "vfs.h"
#include "deviceDriverFs.h"
#include "sdCardFs.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>

#define MAX_FILESYSTEMS (10)
#define DESCRIPTORS_PER_FS  (1024)

static FileSystem_t* fileSystems[MAX_FILESYSTEMS];
static unsigned int fileSystemCount;

typedef struct {
    int filesystem;
    int concreteDescriptor;
} ConcreteDescriptor_t;


static ConcreteDescriptor_t virtualToConcreteDescriptor(int virtualDescriptor) {
    ConcreteDescriptor_t descriptor = { virtualDescriptor / DESCRIPTORS_PER_FS,
                                        virtualDescriptor % DESCRIPTORS_PER_FS };
    return descriptor;
}

static int concreteToVirtualDescriptor(int concreteDescriptor, int fileSystem) {
    return DESCRIPTORS_PER_FS * fileSystem + concreteDescriptor;
}

int vfs_open(const char* fileName) {
    int i;
    for (i = 0; i < fileSystemCount; ++i) {
        int file = fileSystems[i]->open(fileName);
        if (isValidFile(file)) {
            return concreteToVirtualDescriptor(file, i);
        }
    }
    return FILE_NOT_FOUND;
}

void vfs_close(int fileDescriptor) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    fileSystems[descriptor.filesystem]->close(descriptor.concreteDescriptor);
}

int vfs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    return fileSystems[descriptor.filesystem]->read(descriptor.concreteDescriptor, buffer, bufferSize);
}

void vfs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    fileSystems[descriptor.filesystem]->write(descriptor.concreteDescriptor, buffer, bufferSize);
}

void vfs_addFileSystem(FileSystem_t* fileSystem) {
    fileSystems[fileSystemCount++] = fileSystem;
}

const char* vfs_readdir(const char* dirName) {
    static char previousDirectory[100];
    static int currentFs;

    if (strcmp(dirName, previousDirectory) != 0) {
        strncpy(previousDirectory, dirName, sizeof(previousDirectory) - 1);
        currentFs = 0;
    }
    const char* dirEntry;
    do {
        dirEntry = fileSystems[currentFs]->readdir(previousDirectory);
    } while (dirEntry == NULL && ++currentFs < fileSystemCount);

    if (dirEntry == NULL) {
        previousDirectory[0] = '\0';
    }

    return dirEntry;
}

static void initStdStreams() {
    int stdout = vfs_open("/dev/uart3");
}

void vfs_init(void) {
    vfs_addFileSystem(&deviceDriverFs);
    vfs_addFileSystem(&sdCardFs);

    int i;
    for (i = 0; i < fileSystemCount; ++i) {
        fileSystems[i]->init();
    }

    initStdStreams();
}
