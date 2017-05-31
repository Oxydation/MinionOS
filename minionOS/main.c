#include "kernel/systemModules/filesystem/vfs.h"

void main(void) {
    vfs_init();
    int file = vfs_open("/dev/uart3");
    if (file >= 0) {

    }
}
