#include "uartDriver.h"
#include "kernel/hal/uart/uart.h"
#include "vfs.h"
#include "stdio.h"

#define UART1_CONF  "/etc/uart/uart1.conf"
#define UART2_CONF  "/etc/uart/uart2.conf"
#define UART3_CONF  "/etc/uart/uart3.conf"

static void open(UartModule_t module, const char* configFile) {
    int file = vfs_open(configFile);
    if (isValidFile(file)) {
        char buf[100] = {};
        vfs_read(file, (uint8_t*) buf, sizeof(buf));
        UartConfig_t config;
        /* parity  stop  wordL baud  baudM  *
         * 0-4     0-1   0-3   0-xxx 0-1    */
        unsigned int parityMode, stopMode, wordLength, baudRate, baudMultiple;
        sscanf(buf, "%u %u %u %u %u", &parityMode, &stopMode, &wordLength, &baudRate, &baudMultiple);
        config.parityMode = (UartParityMode_t) parityMode;
        config.stopMode = (UartStopMode_t) stopMode;
        config.wordLength = (UartWordLength_t) wordLength;
        config.baudRate = baudRate;
        config.baudMultiple = (UartBaudMultiple_t) baudMultiple;
        uart_initModule(module, config);
        vfs_close(file);
    }
}

static void open1() {
    open(UART1, "/etc/uart/uart1.conf");
}
static void read1(uint8_t* buffer, unsigned int bufferSize) {
    uart_receive(UART1, buffer, bufferSize);
}
static void write1(const uint8_t* buffer, unsigned int bufferSize) {
    uart_transmit(UART1, buffer, bufferSize);
}

static void open2() {
    open(UART2, UART2_CONF);
}
static void read2(uint8_t* buffer, unsigned int bufferSize) {
    uart_receive(UART2, buffer, bufferSize);
}
static void write2(const uint8_t* buffer, unsigned int bufferSize) {
    uart_transmit(UART2, buffer, bufferSize);
}

static void open3() {
    open(UART3, UART3_CONF);
}
static void read3(uint8_t* buffer, unsigned int bufferSize) {
    uart_receive(UART3, buffer, bufferSize);
}
static void write3(const uint8_t* buffer, unsigned int bufferSize) {
    uart_transmit(UART3, buffer, bufferSize);
}

static void release() {
    // no op
}

FileOperations_t devUart1 = { .read = read1, .write = write1, .open = open1, .release = release };
FileOperations_t devUart2 = { .read = read2, .write = write2, .open = open2, .release = release };
FileOperations_t devUart3 = { .read = read3, .write = write3, .open = open3, .release = release };
