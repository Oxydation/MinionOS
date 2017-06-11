#include "cat.h"
#include "applications/systemCallApi.h"
#include "minionIO.h"

int cat_main(int argc, char* argv[]) {
    if (argc == 2) {
        uint8_t buffer[100];
        const char* fileName = argv[1];
        int file = sysCalls_openFile(fileName);
        if (file >= 0) {
            int bytesRead;
            do {
                 bytesRead = sysCalls_readFile(file, buffer, sizeof(buffer));
                 minionIO_writeBytes(buffer, bytesRead);
            } while (!(bytesRead < sizeof(buffer)));
            sysCalls_closeFile(file);
            minionIO_writeln("");
            return 0;
        } else {
            minionIO_writeln("No such file.");
            return -1;
        }
    } else {
        minionIO_writeln("Invalid number of arguments.");
        return -1;
    }
}
