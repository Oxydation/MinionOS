#include "clear.h"
#include "minionIO.h"

int clear_main(int argc, char* argv[]) {
    minionIO_write("\e[2J"); // clear terminal
    minionIO_write("\e[H");  // reset cursor
    return 0;
}
