#include "minionIO.h"
#include "applications/systemCallApi.h"
#include <string.h>

char minionIO_read() {
    uint8_t in;
    sysCalls_readFile(STDIN_FILENO, &in, sizeof(in));
    return in;
}

void minionIO_skipLn(void){
    char c;
    while ((c = minionIO_read()) != EOL) ;
}

int minionIO_readln(char* buffer, unsigned int bufferSize) {
    int i = 0;
    char c;
    while (((bufferSize - 1) > i) && ((c = minionIO_read()) != EOL)) {
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return i;
}

void minionIO_writeBytes(const uint8_t* bytes, unsigned int bytesToWrite) {
    sysCalls_writeFile(STDOUT_FILENO, bytes, bytesToWrite);
}

void minionIO_write(const char* str) {
    int length = strlen(str);
    sysCalls_writeFile(STDOUT_FILENO, (uint8_t*) str, length);
}

void minionIO_writeln(const char* str) {
    minionIO_write(str);
    minionIO_write(EOL_STR);
}
