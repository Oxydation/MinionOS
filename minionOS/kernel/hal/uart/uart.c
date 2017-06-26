#include "uart.h"
#include "global/bitManipulation.h"
#include "kernel/devices/omap3530/includes/uart.h"
#include <inttypes.h>
#include <stdbool.h>
#include "global/delay/delay.h"
#include "kernel/hal/interrupts/interrupts.h"
#include "kernel/devices/omap3530/includes/interrupts.h"

typedef struct {
    uint8_t* MDR1;
    uint8_t* LCR;
    uint8_t* EFR;
    uint8_t* DLL;
    uint8_t* DLH;
    uint8_t* MCR;
    uint8_t* IER;
    uint8_t* TXFLL;
    uint8_t* EBLR;
    uint8_t* ACREG;
    uint8_t* THR;
    uint8_t* LSR;
    uint8_t* SYSC;
    uint8_t* SYSS;
    uint8_t* FCR;
    uint8_t* TLR;
    uint8_t* SCR;
    uint8_t* TCR;
    uint8_t* RHR;
    uint8_t* IIR;
} Uart_t;

#define uartField(name, baseAddress) .name = (uint8_t*) (baseAddress + name##_OFF)
#define createUart(baseAddress) {           \
        uartField(MDR1, baseAddress),   \
        uartField(LCR, baseAddress),    \
        uartField(EFR, baseAddress),    \
        uartField(DLL, baseAddress),    \
        uartField(DLH, baseAddress),    \
        uartField(MCR, baseAddress),    \
        uartField(IER, baseAddress),    \
        uartField(TXFLL, baseAddress),  \
        uartField(EBLR, baseAddress),   \
        uartField(ACREG, baseAddress),  \
        uartField(THR, baseAddress),    \
        uartField(LSR, baseAddress),    \
        uartField(SYSC, baseAddress),   \
        uartField(SYSS, baseAddress),   \
        uartField(FCR, baseAddress),    \
        uartField(TLR, baseAddress),    \
        uartField(SCR, baseAddress),    \
        uartField(TCR, baseAddress),    \
        uartField(RHR, baseAddress),    \
        uartField(IIR, baseAddress)     \
}

static Uart_t modules[3] = { createUart(UART1_BASE), createUart(UART2_BASE),
        createUart(UART3_BASE) };

static void setupProtocolBaudAndInterrupt(Uart_t uart, UartConfig_t config);

static uint16_t calcDivisor(uint64_t baudRate, UartBaudMultiple_t baudMultiple) {
    uint8_t multiple;
    switch (baudMultiple) {
    case x16:
        multiple = 16;
        break;
    case x13:
        multiple = 13;
        break;
    }
    return UART_OPERATING_FREQUENCY / (multiple * baudRate);
}


static void doSwReset(Uart_t uart) {
    // 17.5.1.1.1 Software Reset
    // 1. Initiate a software reset
    bitSet(*uart.SYSC, 1);
    // 2. Wait for the end of the reset operation
    while (!bitRead(*uart.SYSS, 0));
}

static void setupFifoAndDma(Uart_t uart) {
    // 17.5.1.1.2 FIFOs and DMA Settings
    // 1. Switch to register configuration mode B to access the UARTi.EFR_REG register
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x00BF;
    // 2. Enable register submode TCR_TLR to access UARTi.TLR_REG (part 1 of 2)
    uint8_t savedEnhancedEn = bitRead(*uart.EFR, EFR_ENHANCED_EN);
    bitSet(*uart.EFR, EFR_ENHANCED_EN);
    // 3. Switch to register configuration mode A to access the UARTi.MCR_REG register
    *uart.LCR = 0x0080;
    // 4. Enable register submode TCR_TLR to access UARTi.TLR_REG (part 2 of 2)
    uint8_t savedTcrTlr = bitRead(*uart.MCR, MCR_TCR_TLR);
    bitSet(*uart.MCR, MCR_TCR_TLR);
    // 5. Enable FIFO, load the new FIFO triggers (part 1 of 3) and the new DMA mode (part 1 of 2)
    bitClear(*uart.FCR, FCR_RX_FIFO_TRIG_1);
    bitClear(*uart.FCR, FCR_RX_FIFO_TRIG_2);

    bitClear(*uart.FCR, FCR_TX_FIFO_TRIG_1);
    bitClear(*uart.FCR, FCR_TX_FIFO_TRIG_2);

    bitClear(*uart.FCR, FCR_DMA_MODE);

    bitSet(*uart.FCR, FCR_FIFO_ENABLE);
    // 6. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 7. Load the new FIFO triggers (part 2 of 3)
    *uart.TLR = 0x0000;
    // 8. Load the new FIFO triggers (part 3 of 3) and the new DMA mode (part 2 of 2)
    bitSet(*uart.SCR, SCR_TX_TRIG_GRANU1);
    bitSet(*uart.SCR, SCR_DMA_MODE_2_1);
    // 9. Restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 2a
    bitWrite(*uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 10. Switch to register configuration mode A to access the UARTi.MCR_REG register
    *uart.LCR = 0x0080;
    // 11. Restore the UARTi.MCR_REG[6] TCR_TLR value saved in Step 4a
    bitWrite(*uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 12. Restore the UARTi.LCR_REG value saved in Step 1a
    *uart.LCR = savedLcr;
}


void uart_updateConfig(UartModule_t module, UartConfig_t config){
    Uart_t uartModule = modules[module];
    setupProtocolBaudAndInterrupt(uartModule, config);
}

static void setupProtocolBaudAndInterrupt(Uart_t uart, UartConfig_t config) {
    // 17.5.1.1.3 Protocol, Baud Rate, and Interrupt Settings
    // 1. Disable UART to access UARTi.DLL_REG and UARTi.DLH_REG
    *uart.MDR1 = 0x7;
    // 2. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 3. Enable access to UARTi.IER_REG[7:4]
    uint8_t savedEnhancedEn = bitRead(*uart.EFR, EFR_ENHANCED_EN);
    bitSet(*uart.EFR, EFR_ENHANCED_EN);
    // 4. Switch to register operational mode to access the UARTi.IER_REG register
    *uart.LCR = 0x0;
    // 5. Clear the UARTi.IER_REG (UARTi.IER_REG[4] SLEEP_MODE bit to 0 to change UARTi.DLL_REG and UARTi.DLH_REG)
    *uart.IER = 0x0;
    // 6. Switch to register configuration mode B to access the UARTi.DLL_REG and UARTi.DLH_REG registers
    *uart.LCR = 0x00BF;
    // 7. Load the new divisor value
    uint16_t divisor = calcDivisor(config.baudRate, config.baudMultiple);
    *uart.DLL = divisor;
    *uart.DLH = divisor >> 8;
    // 8. Switch to register operational mode to access the UARTi.IER_REG register
    *uart.LCR = 0x0;
    // 9. Load the new interrupt configuration
    bitSet(*uart.IER, 0); // RHR
    // 10. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 11. Restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 3a
    bitWrite(*uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 12. Load the new protocol formatting (parity, stop bit, char length) and switch to register operational mode
    bitClear(*uart.LCR, LCR_DIV_EN);
    bitClear(*uart.LCR, LCR_BREAK_EN);

    switch (config.parityMode) {
    case NO_PARITY:
        bitClear(*uart.LCR, LCR_PARITY_EN);
        break;
    case ODD_PARITY:
        bitSet(*uart.LCR, LCR_PARITY_EN);
        bitClear(*uart.LCR, LCR_PARITY_TYPE1);
        bitClear(*uart.LCR, LCR_PARITY_TYPE2);
        break;
    case EVEN_PARITY:
        bitSet(*uart.LCR, LCR_PARITY_EN);
        bitSet(*uart.LCR, LCR_PARITY_TYPE1);
        bitClear(*uart.LCR, LCR_PARITY_TYPE2);
        break;
    case FORCED_1:
        bitSet(*uart.LCR, LCR_PARITY_EN);
        bitClear(*uart.LCR, LCR_PARITY_TYPE1);
        bitSet(*uart.LCR, LCR_PARITY_TYPE2);
        break;
    case FORCED_0:
        bitSet(*uart.LCR, LCR_PARITY_EN);
        bitSet(*uart.LCR, LCR_PARITY_TYPE1);
        bitSet(*uart.LCR, LCR_PARITY_TYPE2);
        break;
    }

    switch (config.wordLength) {
    case LENGTH_5:
        bitClear(*uart.LCR, LCR_CHAR_LENGTH_1);
        bitClear(*uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_6:
        bitSet(*uart.LCR, LCR_CHAR_LENGTH_1);
        bitClear(*uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_7:
        bitClear(*uart.LCR, LCR_CHAR_LENGTH_1);
        bitSet(*uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_8:
        bitSet(*uart.LCR, LCR_CHAR_LENGTH_1);
        bitSet(*uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    }

    switch (config.stopMode) {
    case STOP_1:
        bitClear(*uart.LCR, LCR_NB_STOP);
        break;
    case STOP_1_5:
        bitSet(*uart.LCR, LCR_NB_STOP);
        break;
    }
    // 13. Load the new UART mode
    bitClear(*uart.MDR1, MDR1_MODE_SELECT_1);
    bitClear(*uart.MDR1, MDR1_MODE_SELECT_2);
    bitClear(*uart.MDR1, MDR1_MODE_SELECT_3);
}

static void setupHwFlowControl(Uart_t uart) {
    // 17.5.1.2.1 Hardware Flow Control Configuration
    // 1. Switch to register configuration mode A to access the UARTi.MCR_REG register
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x0080;
    // 2. Enable register submode TCR_TLR to access UARTi.TCR_REG (part 1 of 2)
    uint8_t savedTcrTlr = bitRead(*uart.MCR, MCR_TCR_TLR);
    bitSet(*uart.MCR, MCR_TCR_TLR);
    // 3. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 4. Enable register submode TCR_TLR to access the UARTi.TCR_REG register (part 2 of 2)
    uint8_t savedEnhancedEn = bitRead(*uart.EFR, EFR_ENHANCED_EN);
    bitSet(*uart.EFR, EFR_ENHANCED_EN);
    // 5. Load the new start and halt trigger values for hardware flow control
    bitClear(*uart.TCR, 0);
    bitClear(*uart.TCR, 1);
    bitClear(*uart.TCR, 2);
    bitClear(*uart.TCR, 3);
    bitSet(*uart.TCR, 4);
    bitSet(*uart.TCR, 5);
    bitSet(*uart.TCR, 6);
    bitSet(*uart.TCR, 7);
    // 6. Enable or disable receive and transmit hardware flow control mode and restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 4a
    bitSet(*uart.EFR, EFR_AUTO_CTS_EN);
    bitSet(*uart.EFR, EFR_AUTO_RTS_EN);
    bitWrite(*uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 7. Switch to register configuration mode A to access UARTi.MCR_REG
    *uart.LCR = 0x0080;
    // 8. Restore the UARTi.MCR_REG[6] TCR_TLR value saved in Step 2a
    bitWrite(*uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 9. Restore the UARTi.LCR_REG value saved in Step 1a
    *uart.LCR = savedLcr;
}

uint8_t data[150;
static void isr_handler(uint32_t source, PCB_t * currentPcb)
{
    UartModule_t module = uart_getModuleFromIrqSource(source);
    //Uart_t uartModule = modules[module];
    //g_timer[timerNumber].callback(currentPcb);

    uart_read(module, &data);
}

void uart_initModule(UartModule_t module, UartConfig_t config) {
    Uart_t uartModule = modules[module];
    doSwReset(uartModule);
    setupFifoAndDma(uartModule);
    setupProtocolBaudAndInterrupt(uartModule, config);
    setupHwFlowControl(uartModule);

    switch(module){
    case UART1:
        interrupts_registerHandler(&isr_handler, UART1_IRQ);
        break;
    case UART2:
        interrupts_registerHandler(&isr_handler, UART2_IRQ);
        break;
    case UART3:
        interrupts_registerHandler(&isr_handler, UART3_IRQ);
        break;
    }
}

static bool isReadyToTransmit(Uart_t uartModule) {
    return bitRead(*uartModule.LSR, LSR_TX_FIFO_E);
}

void uart_transmit(UartModule_t module, const uint8_t* buffer, uint32_t bufferSize) {
    Uart_t uartModule = modules[module];
    int i;
    for (i = 0; i < bufferSize; i++) {
        while (!isReadyToTransmit(uartModule));
        *uartModule.THR = buffer[i];
        delay(15);
    }
}

static bool hasReceived(Uart_t uartModule) {
    return bitRead(*uartModule.LSR, LSR_RX_FIFO_E);
}

bool uart_resetFifo(UartModule_t module){
    Uart_t uartModule = modules[module];
    return bitSet(*uartModule.FCR, 1);
}

void uart_receive(UartModule_t module, uint8_t* buffer, uint32_t bufferSize) {
    // TODO buffer overflow?
    Uart_t uartModule = modules[module];
    int i;
    for (i = 0; i < bufferSize; i++) {
        while (!hasReceived(uartModule));
        buffer[i] = *uartModule.RHR;
    }
}

void uart_write(UartModule_t module, uint8_t c) {
    Uart_t uartModule = modules[module];
    *uartModule.THR = c;
}

void uart_read(UartModule_t module, uint8_t * c) {
    Uart_t uartModule = modules[module];
    *c = *uartModule.RHR;
}

void uart_enableBreak(UartModule_t module) {
    Uart_t uartModule = modules[module];
    bitSet(*uartModule.LCR, LCR_BREAK_EN);
}

void uart_disableBreak(UartModule_t module) {
    Uart_t uartModule = modules[module];
    bitClear(*uartModule.LCR, LCR_BREAK_EN);
}

uint8_t uart_getInterruptType(UartModule_t module) {
    Uart_t uartModule = modules[module];
    return getBitRange(*uartModule.IIR, 5, 1);
}
