#include "shell.h"
#include "applications/systemCallApi.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define EOL_STR     "\r\n"
#define EOL         '\r'
#define ARG_DELIMITER   " "

#define MAX_LINE_LENGTH     100
#define MAX_TOKENS          11

#define MAX_TOKEN_EXCEEDED  -1

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

int stdoutFile, stdinFile;

static char read() {
    uint8_t in;
    sysCalls_readFile(stdinFile, &in, sizeof(in));
    return in;
}

static int readln(char* buffer, unsigned int bufferSize) {
    int i = 0;
    char c;
    while (((bufferSize - 1) > i) && ((c = read()) != EOL)) {
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return i;
}

static void write(const char* str) {
    int length = strlen(str);
    sysCalls_writeFile(stdoutFile, (uint8_t*) str, length);
}

static void writeln(const char* str) {
    write(str);
    write(EOL_STR);
}

static int tokenize(char lineBuffer[], char* tokenBuffer[]) {
    int i = 0;
    tokenBuffer[i++] = strtok(lineBuffer, ARG_DELIMITER);
    while ((i < MAX_TOKENS) && (tokenBuffer[i - 1] != NULL)) {
        tokenBuffer[i++] = strtok(NULL, ARG_DELIMITER);
    }
    return tokenBuffer[i - 1] == NULL ? i - 1 : MAX_TOKEN_EXCEEDED;
}

static int execute(int argc, char* argv[]) {
    char buf[80];
    sprintf(buf, "%d arguments read.", argc);
    writeln(buf);
    int i;
    for (i = 0; i < argc; ++i) {
        writeln(argv[i]);
    }
}

static void flushIn(char buffer[], int bufferSize) {
    // read and discard rest of line that was too long
    while (readln(buffer, bufferSize) >= MAX_LINE_LENGTH);
}

void shell_loop() {
    stdoutFile = stdinFile = sysCalls_openFile("/dev/uart3");

    if (stdoutFile < 0) {
        return;
    }

    char lineBuffer[MAX_LINE_LENGTH + 1];
    char* tokenBuffer[MAX_TOKENS + 1];

    // TODO send control chars to notify putty about local echo and local line edit
    while (1) {
        write("> ");

        // Read line with max 100 characters, else discard whole line
        int charsRead = readln(lineBuffer, ARRAY_SIZE(lineBuffer));
        if (charsRead >= MAX_LINE_LENGTH) {
            writeln("Error: Line length must not exceed 100 characters.");
            flushIn(lineBuffer, ARRAY_SIZE(lineBuffer));
            continue;
        }

        // Tokenize line with max 10 tokens, else discard line
        int argc = tokenize(lineBuffer, tokenBuffer);
        if (argc == MAX_TOKEN_EXCEEDED) {
            writeln("Error: Number of arguments must not exceed 10.");
            continue;
        }

        if (argc > 0) {
            execute(argc, tokenBuffer);
        }
    }
}
