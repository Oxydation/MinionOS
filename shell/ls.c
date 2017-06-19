#include "ls.h"
#include "systemCallApi.h"
#include "minionIO.h"
#include <stddef.h>

int ls_main(int argc, char* argv[]) {
    const char* dirName = argc > 1 ? argv[1] : "";
    const char* dirEntry;
    while ((dirEntry = sysCalls_readDirectory(dirName)) != NULL) {
        minionIO_writeln(dirEntry);
    }
    return 0;
}
