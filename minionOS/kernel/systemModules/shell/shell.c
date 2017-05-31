#include "shell.h"
#include "kernel/systemModules/filesystem/vfs.h"
#include <string.h>
#include <assert.h>

#define EOL_STR     "\r\n"
#define EOL         '\r'
#define DELIMITER   " "

int stdout, stdin;

char shell_read() {
    uint8_t in;
    vfs_read(stdin, &in, sizeof(in));
    return in;
}

int shell_readln(char* buffer, unsigned int bufferSize) {
    int i = 0;
    char c;
    while (((bufferSize - 1) > i) && ((c = shell_read()) != EOL)) {
        /*if (c == '\n') {
            continue;
        }*/
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return i;
}

void shell_write(const char* str) {
    int length = strlen(str);
    vfs_write(stdout, (uint8_t*) str, length);
}

void shell_writeln(const char* str) {
    shell_write(str);
    shell_write(EOL_STR);
}

#define MAX_LINE_LENGTH 100
#define MAX_TOKENS      11

void shell_loop() {
    stdout = stdin = vfs_open("/dev/uart3");

    if (!isValidFile(stdout)) {
        return;
    }

    char lineBuffer[MAX_LINE_LENGTH + 1];
    char* tokenBuffer[MAX_TOKENS + 1];

    // TODO send control chars to notify putty about local echo and local line edit
    while (1) {
        shell_write("> ");
        int charsRead = shell_readln(lineBuffer, sizeof(lineBuffer));
        if (charsRead >= MAX_LINE_LENGTH) {
            shell_writeln("Error: Line length must not exceed 100 characters.");
            // read and discard rest of line that was too long
            while (shell_readln(lineBuffer, sizeof(lineBuffer)) >= MAX_LINE_LENGTH);
            continue;
        }

        int i = 0;

        tokenBuffer[i++] = strtok(lineBuffer, DELIMITER);
        while ((i < MAX_TOKENS) && (tokenBuffer[i - 1] != NULL)) {
            tokenBuffer[i++] = strtok(NULL, DELIMITER);
        }
        if (tokenBuffer[i - 1] != NULL) {
            shell_writeln("Error: Number of arguments must not exceed 10.");
            continue;
        }

        for (i = 0; (i < sizeof(tokenBuffer)) && (tokenBuffer[i] != NULL); ++i) {
            shell_writeln(tokenBuffer[i]);
        }
    }
}
