#ifndef KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_
#define KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_

#include <inttypes.h>

#define FILE_NOT_FOUND (-1)

typedef struct {
    int (*open)(const char* fileName);
    void (*close)(const int fileDescriptor);
    void (*read)(const int fileDescriptor, uint8_t* buffer, unsigned int bufferSize);
    void (*write)(const int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize);
    void (*init)(void);
} FileSystem_t;

int vfs_open(const char* fileName);

void vfs_close(int fileDescriptor);

void vfs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize);

void vfs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize);

void vfs_init(void);

#endif /* KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_ */
