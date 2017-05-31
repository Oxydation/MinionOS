#include "vfs.h"
#include "deviceDriverFs.h"
#include "confFs.h"
#include <limits.h>

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
        if (file >= 0) {
            return concreteToVirtualDescriptor(file, i);
        }
    }
    return FILE_NOT_FOUND;
}

void vfs_close(int fileDescriptor) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    fileSystems[descriptor.filesystem]->close(descriptor.concreteDescriptor);
}

void vfs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    fileSystems[descriptor.filesystem]->read(descriptor.concreteDescriptor, buffer, bufferSize);
}

void vfs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize) {
    ConcreteDescriptor_t descriptor = virtualToConcreteDescriptor(fileDescriptor);
    fileSystems[descriptor.filesystem]->write(descriptor.concreteDescriptor, buffer, bufferSize);
}

void vfs_addFileSystem(FileSystem_t* fileSystem) {
    fileSystems[fileSystemCount++] = fileSystem;
}

void vfs_init(void) {
    vfs_addFileSystem(&deviceDriverFs);
    vfs_addFileSystem(&confFs);
    int i;
    for (i = 0; i < fileSystemCount; ++i) {
        fileSystems[i]->init();
    }
}
