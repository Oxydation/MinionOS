#include "argv.h"
#include "minionIO.h"
#include <stdio.h>

int argv_main(int argc, char* argv[]) {
    char buf[80];
    sprintf(buf, "%d arguments read.", argc);
    minionIO_writeln(buf);
    int i;
    for (i = 0; i < argc; ++i) {
        minionIO_writeln(argv[i]);
    }
    return 0;
}
