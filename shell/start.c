#include "start.h"
#include "systemCallApi.h"
#include "minionIO.h"

int start_main(int argc, char* argv[]) {
    if (argc != 2) {
        minionIO_writeln("Expected one argument containing the path of the program.");
        return -1;
    }
    return sysCalls_loadProgramm(argv[1]);
}
