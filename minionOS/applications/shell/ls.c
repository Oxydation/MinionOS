#include "ls.h"
#include "applications/systemCallApi.h"
#include "minionIO.h"
#include <stddef.h>
#include <string.h>

int ls_main(int argc, char* argv[]) {
    const char* dirName = argc > 1 ? argv[1] : "";
    const char* dirEntry;
    if (dirName[0] != '/' || dirName[strlen(dirName) - 1] != '/') {
        minionIO_writeln("Path must start and end with '/'.");
        return -1;
    }
    while ((dirEntry = sysCalls_readDirectory(dirName)) != NULL) {
        minionIO_writeln(dirEntry);
    }
    return 0;
}
