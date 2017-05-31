#include "kernel/systemModules/filesystem/vfs.h"
#include "kernel/systemModules/shell/shell.h"

void main(void) {
    vfs_init();

    shell_loop();
}
