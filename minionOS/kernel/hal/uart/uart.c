#include "uart.h"
#include <inttypes.h>
#include <stdbool.h>

// Base addresses
#define UART1_BASE       0x4806A000
#define UART2_BASE       0x4806C000
#define UART3_BASE       0x49020000

#define UART_OPERATING_FREQUENCY    48000000

// Register offsets
#define DLL_OFF     0x0
#define DLH_OFF     0x4
#define MDR1_OFF    0x20
#define LCR_OFF     0xC
#define EFR_OFF     0x8
#define MCR_OFF     0x10
#define IER_OFF     0x4
#define TXFLL_OFF   0x28
#define EBLR_OFF    0x48
#define ACREG_OFF   0x3C
#define THR_OFF     0x0
#define LSR_OFF     0x14
#define SYSC_OFF    0x54
#define SYSS_OFF    0x58
#define FCR_OFF     0x8
#define TLR_OFF     0x01
#define SCR_OFF     0x40
#define TCR_OFF     0x18
#define RHR_OFF     0x0

// Indices for register fields
#define EFR_ENHANCED_EN     4
#define EFR_AUTO_CTS_EN     7
#define EFR_AUTO_RTS_EN     6

#define MCR_TCR_TLR         6

#define FCR_RX_FIFO_TRIG_1  6
#define FCR_RX_FIFO_TRIG_2  7
#define FCR_TX_FIFO_TRIG_1  4
#define FCR_TX_FIFO_TRIG_2  5
#define FCR_DMA_MODE        3
#define FCR_FIFO_ENABLE     0

#define SCR_TX_TRIG_GRANU1  6
#define SCR_DMA_MODE_2_1    1

#define LCR_PARITY_EN       3
#define LCR_PARITY_TYPE2    5
#define LCR_PARITY_TYPE1    4
#define LCR_DIV_EN          7
#define LCR_BREAK_EN        6
#define LCR_CHAR_LENGTH_1   0
#define LCR_CHAR_LENGTH_2   1
#define LCR_NB_STOP         2

#define MDR1_MODE_SELECT_1  0
#define MDR1_MODE_SELECT_2  1
#define MDR1_MODE_SELECT_3  2

#define LSR_TX_FIFO_E       5
#define LSR_RX_FIFO_E       0

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
} UART_t;

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
        uartField(RHR, baseAddress)     \
}

UART_t modules[3] = { createUart(UART1_BASE), createUart(UART2_BASE),
        createUart(UART3_BASE) };

void setBit(uint8_t* address, uint8_t bit) {
    *address |= 1 << bit;
}

uint8_t getBit(uint8_t* address, uint8_t bit) {
    return (*address >> bit) & 0x1;
}

void clearBit(uint8_t* address, uint8_t bit) {
    *address &= ~(1 << bit);
}

void restoreBit(uint8_t* address, uint8_t bit, uint8_t savedBit) {
    *address &= ~(1 << bit);
    *address |= (savedBit << bit);
}

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

void doSwReset(UART_t uart) {
    // 17.5.1.1.1 Software Reset
    // 1. Initiate a software reset
    setBit(uart.SYSC, 1);
    // 2. Wait for the end of the reset operation
    while (!getBit(uart.SYSS, 0));
}

static void setupFifoAndDma(UART_t uart) {
    // 17.5.1.1.2 FIFOs and DMA Settings
    // 1. Switch to register configuration mode B to access the UARTi.EFR_REG register
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x00BF;
    // 2. Enable register submode TCR_TLR to access UARTi.TLR_REG (part 1 of 2)
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
    // 3. Switch to register configuration mode A to access the UARTi.MCR_REG register
    *uart.LCR = 0x0080;
    // 4. Enable register submode TCR_TLR to access UARTi.TLR_REG (part 2 of 2)
    uint8_t savedTcrTlr = getBit(uart.MCR, MCR_TCR_TLR);
    setBit(uart.MCR, MCR_TCR_TLR);
    // 5. Enable FIFO, load the new FIFO triggers (part 1 of 3) and the new DMA mode (part 1 of 2)
    clearBit(uart.FCR, FCR_RX_FIFO_TRIG_1);
    clearBit(uart.FCR, FCR_RX_FIFO_TRIG_2);

    clearBit(uart.FCR, FCR_TX_FIFO_TRIG_1);
    clearBit(uart.FCR, FCR_TX_FIFO_TRIG_2);

    clearBit(uart.FCR, FCR_DMA_MODE);

    setBit(uart.FCR, FCR_FIFO_ENABLE);
    // 6. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 7. Load the new FIFO triggers (part 2 of 3)
    *uart.TLR = 0x0000;
    // 8. Load the new FIFO triggers (part 3 of 3) and the new DMA mode (part 2 of 2)
    setBit(uart.SCR, SCR_TX_TRIG_GRANU1);
    setBit(uart.SCR, SCR_DMA_MODE_2_1);
    // 9. Restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 2a
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 10. Switch to register configuration mode A to access the UARTi.MCR_REG register
    *uart.LCR = 0x0080;
    // 11. Restore the UARTi.MCR_REG[6] TCR_TLR value saved in Step 4a
    restoreBit(uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 12. Restore the UARTi.LCR_REG value saved in Step 1a
    *uart.LCR = savedLcr;
}

static void setupProtocolBaudAndInterrupt(UART_t uart, UartConfig_t config) {
    // 17.5.1.1.3 Protocol, Baud Rate, and Interrupt Settings
    // 1. Disable UART to access UARTi.DLL_REG and UARTi.DLH_REG
    *uart.MDR1 = 0x7;
    // 2. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 3. Enable access to UARTi.IER_REG[7:4]
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
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
    // no interrupts
    // 10. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 11. Restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 3a
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 12. Load the new protocol formatting (parity, stop bit, char length) and switch to register operational mode
    clearBit(uart.LCR, LCR_DIV_EN);
    clearBit(uart.LCR, LCR_BREAK_EN);

    switch (config.parityMode) {
    case NO_PARITY:
        clearBit(uart.LCR, LCR_PARITY_EN);
        break;
    case ODD_PARITY:
        setBit(uart.LCR, LCR_PARITY_EN);
        clearBit(uart.LCR, LCR_PARITY_TYPE1);
        clearBit(uart.LCR, LCR_PARITY_TYPE2);
        break;
    case EVEN_PARITY:
        setBit(uart.LCR, LCR_PARITY_EN);
        setBit(uart.LCR, LCR_PARITY_TYPE1);
        clearBit(uart.LCR, LCR_PARITY_TYPE2);
        break;
    case FORCED_1:
        setBit(uart.LCR, LCR_PARITY_EN);
        clearBit(uart.LCR, LCR_PARITY_TYPE1);
        setBit(uart.LCR, LCR_PARITY_TYPE2);
        break;
    case FORCED_0:
        setBit(uart.LCR, LCR_PARITY_EN);
        setBit(uart.LCR, LCR_PARITY_TYPE1);
        setBit(uart.LCR, LCR_PARITY_TYPE2);
        break;
    }

    switch (config.wordLength) {
    case LENGTH_5:
        clearBit(uart.LCR, LCR_CHAR_LENGTH_1);
        clearBit(uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_6:
        setBit(uart.LCR, LCR_CHAR_LENGTH_1);
        clearBit(uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_7:
        clearBit(uart.LCR, LCR_CHAR_LENGTH_1);
        setBit(uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    case LENGTH_8:
        setBit(uart.LCR, LCR_CHAR_LENGTH_1);
        setBit(uart.LCR, LCR_CHAR_LENGTH_2);
        break;
    }

    switch (config.stopMode) {
    case STOP_1:
        clearBit(uart.LCR, LCR_NB_STOP);
        break;
    case STOP_1_5:
        setBit(uart.LCR, LCR_NB_STOP);
        break;
    }
    // 13. Load the new UART mode
    clearBit(uart.MDR1, MDR1_MODE_SELECT_1);
    clearBit(uart.MDR1, MDR1_MODE_SELECT_2);
    clearBit(uart.MDR1, MDR1_MODE_SELECT_3);
}

static void setupHwFlowControl(UART_t uart) {
    // 17.5.1.2.1 Hardware Flow Control Configuration
    // 1. Switch to register configuration mode A to access the UARTi.MCR_REG register
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x0080;
    // 2. Enable register submode TCR_TLR to access UARTi.TCR_REG (part 1 of 2)
    uint8_t savedTcrTlr = getBit(uart.MCR, MCR_TCR_TLR);
    setBit(uart.MCR, MCR_TCR_TLR);
    // 3. Switch to register configuration mode B to access the UARTi.EFR_REG register
    *uart.LCR = 0x00BF;
    // 4. Enable register submode TCR_TLR to access the UARTi.TCR_REG register (part 2 of 2)
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
    // 5. Load the new start and halt trigger values for hardware flow control
    clearBit(uart.TCR, 0);
    clearBit(uart.TCR, 1);
    clearBit(uart.TCR, 2);
    clearBit(uart.TCR, 3);
    setBit(uart.TCR, 4);
    setBit(uart.TCR, 5);
    setBit(uart.TCR, 6);
    setBit(uart.TCR, 7);
    // 6. Enable or disable receive and transmit hardware flow control mode and restore the UARTi.EFR_REG[4] ENHANCED_EN value saved in Step 4a
    setBit(uart.EFR, EFR_AUTO_CTS_EN);
    setBit(uart.EFR, EFR_AUTO_RTS_EN);
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 7. Switch to register configuration mode A to access UARTi.MCR_REG
    *uart.LCR = 0x0080;
    // 8. Restore the UARTi.MCR_REG[6] TCR_TLR value saved in Step 2a
    restoreBit(uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 9. Restore the UARTi.LCR_REG value saved in Step 1a
    *uart.LCR = savedLcr;
}

void initModule(UartModule_t module, UartConfig_t config) {
    UART_t uartModule = modules[module];
    doSwReset(uartModule);
    setupFifoAndDma(uartModule);
    setupProtocolBaudAndInterrupt(uartModule, config);
    setupHwFlowControl(uartModule);
}

static bool isReadyToTransmit(UART_t uartModule) {
    return getBit(uartModule.LSR, LSR_TX_FIFO_E);
}

void transmit(UartModule_t module, const uint8_t* buffer, uint32_t bufferSize) {
    // TODO buffer overflow?
    UART_t uartModule = modules[module];
    int i;
    for (i = 0; i < bufferSize; i++) {
        while (!isReadyToTransmit(uartModule));
        *uartModule.THR = buffer[i];
    }
}

static bool hasReceived(UART_t uartModule) {
    return getBit(uartModule.LSR, LSR_RX_FIFO_E);
}

void receive(UartModule_t module, uint8_t* buffer, uint32_t bufferSize) {
    // TODO buffer overflow?
    UART_t uartModule = modules[module];
    int i;
    for (i = 0; i < bufferSize; i++) {
        while (!hasReceived(uartModule));
        buffer[i] = *uartModule.RHR;
    }
}

void enableModule(UartModule_t module) {
    UART_t uartModule = modules[module];
    uartModule.MDR1 &= ~0x7;
}

void disableModule(UartModule_t module) {
    UART_t uartModule = modules[module];
    uartModule.MDR1 |= 0x7;
}
