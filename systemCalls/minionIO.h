#ifndef APPLICATIONS_SHELL_MINIONIO_H_
#define APPLICATIONS_SHELL_MINIONIO_H_

#include <inttypes.h>

#define STDOUT_FILENO  (2)
#define STDIN_FILENO   (2)

#define EOL         '\r'
#define EOL_STR     "\r\n"

char minionIO_read();

int minionIO_readln(char* buffer, unsigned int bufferSize);

void minionIO_writeBytes(const uint8_t* bytes, unsigned int bytesToWrite);

void minionIO_write(const char* str);

void minionIO_writeln(const char* str);

void minionIO_skipLn(void);

#endif /* APPLICATIONS_SHELL_MINIONIO_H_ */
