#include "write.h"
#include "minionIO.h"
#include "systemCallApi.h"
#include <string.h>

int write_main(int argc, char* argv[]) {
    if (argc != 3) {
        minionIO_writeln("Incorrect usage. Use 'write <targetFile> <content>'.");
        return -1;
    }
    const char* fileName = argv[1];
    const char* content = argv[2];

    int file = sysCalls_openFile(fileName);
    if (file < 0) {
        minionIO_writeln("Specified file does not exist.");
        return -1;
    }
    sysCalls_writeFile(file, (const uint8_t*) content, strlen(content));
    sysCalls_closeFile(file);
    return 0;
}
