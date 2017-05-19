#ifndef __UART
#define __UART

typedef enum {
    UART1,
    UART2,
    UART3
} UartModule_t;

typedef struct {

} UartConfig_t;


void receive(UartModule_t module, const uint8_t* buffer, uint32_t bufferSize);

void transmit(UartModule_t module, const uint8_t* buffer, uint32_t bufferSize);

void initModule(UartModule_t module, UartConfig_t config);

#endif
