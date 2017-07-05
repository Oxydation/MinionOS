#include "shell.h"
#include "systemCallApi.h"
#include "minionIO.h"
#include "ls.h"
#include "start.h"
#include "clear.h"
#include "argv.h"
#include "read.h"
#include "write.h"
#include <string.h>
#include <stdio.h>

#define ARG_DELIMITER   " "

#define MAX_LINE_LENGTH     100
#define MAX_TOKENS          11

#define MAX_TOKEN_EXCEEDED  -1

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define MAX_COMMAND_NAME_LENGTH     10
#define MAX_COMMANDS                10

typedef int (*MainFunc_t)(int argc, char* argv[]);

typedef struct {
    char name[MAX_COMMAND_NAME_LENGTH + 1];
    MainFunc_t main;
} Command_t;

static Command_t commands[MAX_COMMANDS];
static unsigned int commandCount;

static int tokenize(char lineBuffer[], char* tokenBuffer[]) {
    int i = 0;
    tokenBuffer[i++] = strtok(lineBuffer, ARG_DELIMITER);
    while ((i < MAX_TOKENS) && (tokenBuffer[i - 1] != NULL)) {
        tokenBuffer[i++] = strtok(NULL, ARG_DELIMITER);
    }
    return tokenBuffer[i - 1] == NULL ? i - 1 : MAX_TOKEN_EXCEEDED;
}

static int execute(int argc, char* argv[]) {
    int i;
    for (i = 0; i < commandCount; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            return commands[i].main(argc, argv);
        }
    }
    minionIO_writeln("Unknown command.");
    return -1;
}

static void flushIn(char buffer[], int bufferSize) {
    // read and discard rest of line that was too long
    while (minionIO_readln(buffer, bufferSize) >= MAX_LINE_LENGTH);
}

static void registerCommand(const char* name, MainFunc_t mainFunc) {
    if (commandCount < MAX_COMMANDS) {
        Command_t* command = &commands[commandCount++];
        strcpy(command->name, name);
        command->main = mainFunc;
    } else {
        minionIO_writeln("Tried to add command, but no more slots left.");
    }
}

static void initCommands() {
    registerCommand("ls", ls_main);
    registerCommand("read", read_main);
    registerCommand("start", start_main);
    registerCommand("argv", argv_main);
    registerCommand("clear", clear_main);
    registerCommand("write", write_main);
}

void shell_loop() {
    char lineBuffer[MAX_LINE_LENGTH + 1];
    char* tokenBuffer[MAX_TOKENS + 1];

    initCommands();

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
