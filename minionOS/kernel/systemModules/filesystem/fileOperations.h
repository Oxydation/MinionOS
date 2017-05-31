#ifndef KERNEL_SYSTEMMODULES_FILESYSTEM_FILEOPERATIONS_H_
#define KERNEL_SYSTEMMODULES_FILESYSTEM_FILEOPERATIONS_H_

#include <inttypes.h>

typedef struct {
    void (*read)(uint8_t* buffer, unsigned int bufferSize);
    void (*write)(const uint8_t* buffer, unsigned int bufferSize);
    void (*open)(void);
    void (*release)(void);
} FileOperations_t;

#endif /* KERNEL_SYSTEMMODULES_FILESYSTEM_FILEOPERATIONS_H_ */
