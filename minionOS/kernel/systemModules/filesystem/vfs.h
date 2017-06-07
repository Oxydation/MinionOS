#ifndef KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_
#define KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_

#include <inttypes.h>

#define isValidFile(fileDescriptor) (fileDescriptor >= 0)
#define FILE_NOT_FOUND (-1)

typedef struct {
    int (*open)(const char* fileName);
    void (*close)(const int fileDescriptor);
    int (*read)(const int fileDescriptor, uint8_t* buffer, unsigned int bufferSize);
    void (*write)(const int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize);
    const char* (*readdir)(const char* dirName);
    void (*init)(void);
} FileSystem_t;

/**
 * Tries to open the file with the given absolute name and returns a file descriptor,
 * which should be used for further operations on this file.
 */
int vfs_open(const char* fileName);

/**
 * Closes the file by releasing any resources associated with it and destroying
 * the respective file descriptor.
 */
void vfs_close(int fileDescriptor);

/**
 * Reads as many bytes as possible from the current position of the file
 * into the buffer (either limited by the file's size or the specified buffer size).
 * Returns the number of actually read bytes.
 */
int vfs_read(int fileDescriptor, uint8_t* buffer, unsigned int bufferSize);

/**
 * Writes the contents of the provided buffer to the specified file.
 */
void vfs_write(int fileDescriptor, const uint8_t* buffer, unsigned int bufferSize);

/**
 * Reads the content of the directory with the specified name. Each call to this
 * function with the same arguments returns the next entry of the directory
 * until all entries have been read and NULL is returned.
 */
const char* vfs_readdir(const char* dirName);

/**
 * Initializes the virtual filesystem and all known sub-filesystems.
 */
void vfs_init(void);

#endif /* KERNEL_SYSTEMMODULES_FILESYSTEM_VFS_H_ */
