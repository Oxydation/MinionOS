#include "shell.h"
#include "systemCallApi.h"
#include "minionIO.h"
#include "ls.h"
#include "cat.h"
#include "start.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define ARG_DELIMITER   " "

#define MAX_LINE_LENGTH     100
#define MAX_TOKENS          11

#define MAX_TOKEN_EXCEEDED  -1

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

static int tokenize(char lineBuffer[], char* tokenBuffer[]) {
    int i = 0;
    tokenBuffer[i++] = strtok(lineBuffer, ARG_DELIMITER);
    while ((i < MAX_TOKENS) && (tokenBuffer[i - 1] != NULL)) {
        tokenBuffer[i++] = strtok(NULL, ARG_DELIMITER);
    }
    return tokenBuffer[i - 1] == NULL ? i - 1 : MAX_TOKEN_EXCEEDED;
}

static int execute(int argc, char* argv[]) {
    if (strcmp(argv[0], "clear") == 0) {
        minionIO_write("\e[2J"); // clear terminal
        minionIO_write("\e[H");  // reset cursor
        return 0;
    } else if (strcmp(argv[0], "ls") == 0) {
        return ls_main(argc, argv);
    } else if (strcmp(argv[0], "cat") == 0) {
        return cat_main(argc, argv);
    } else if (strcmp(argv[0], "argv") == 0) {
        char buf[80];
        sprintf(buf, "%d arguments read.", argc);
        minionIO_writeln(buf);
        int i;
        for (i = 0; i < argc; ++i) {
            minionIO_writeln(argv[i]);
        }
        return 0;
    } else if (strcmp(argv[0], "start" == 0)) {
        return start_main(argc, argv);
    } else if (argc > 0) {
        minionIO_writeln("Unknown command.");
    }
    return -1;
}

static void flushIn(char buffer[], int bufferSize) {
    // read and discard rest of line that was too long
    while (minionIO_readln(buffer, bufferSize) >= MAX_LINE_LENGTH);
}

void shell_loop() {
    char lineBuffer[MAX_LINE_LENGTH + 1];
    char* tokenBuffer[MAX_TOKENS + 1];

    // TODO send control chars to notify putty about local echo and local line edit
    minionIO_writeln("");
    minionIO_writeln("Minion OS Shell v1.0 initialized.");
    while (1) {
        minionIO_write("> ");

        // Read line with max 100 characters, else discard whole line
        int charsRead = minionIO_readln(lineBuffer, ARRAY_SIZE(lineBuffer));
        if (charsRead >= MAX_LINE_LENGTH) {
            minionIO_writeln("Error: Line length must not exceed 100 characters.");
            flushIn(lineBuffer, ARRAY_SIZE(lineBuffer));
            continue;
        }

        // Tokenize line with max 10 tokens, else discard line
        int tokenizeResult = tokenize(lineBuffer, tokenBuffer);
        if (tokenizeResult == MAX_TOKEN_EXCEEDED) {
            minionIO_writeln("Error: Number of arguments must not exceed 10.");
            continue;
        }

        if (tokenizeResult > 0) {
            execute(tokenizeResult, tokenBuffer);
        }
    }
}
