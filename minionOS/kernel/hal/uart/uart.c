#include "uart.h"
#include <inttypes.h>

// Base addresses
#define UART1_BASE       0x4806A000
#define UART2_BASE       0x4806C000
#define UART3_BASE       0x49020000

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

void swReset(UART_t uart) {
    // Software Reset
    // 1
    setBit(uart.SYSC, 1);
    // 2
    while (!getBit(uart.SYSS, 0));
}

void fifoAndDmaSettings(UART_t uart) {
    // FIFOs and DMA Settings
    // 1
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x00BF;
    // 2
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
    // 3
    *uart.LCR = 0x0080;
    // 4
    uint8_t savedTcrTlr = getBit(uart.MCR, MCR_TCR_TLR);
    setBit(uart.MCR, MCR_TCR_TLR);
    // 5
    clearBit(uart.FCR, FCR_RX_FIFO_TRIG_1);
    clearBit(uart.FCR, FCR_RX_FIFO_TRIG_2);

    clearBit(uart.FCR, FCR_TX_FIFO_TRIG_1);
    clearBit(uart.FCR, FCR_TX_FIFO_TRIG_2);

    clearBit(uart.FCR, FCR_DMA_MODE);

    setBit(uart.FCR, FCR_FIFO_ENABLE);
    // 6
    *uart.LCR = 0x00BF;
    // 7
    *uart.TLR = 0x0000;
    // 8
    setBit(uart.SCR, SCR_TX_TRIG_GRANU1);
    setBit(uart.SCR, SCR_DMA_MODE_2_1);
    // 9
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 10
    *uart.LCR = 0x0080;
    // 11
    restoreBit(uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 12
    *uart.LCR = savedLcr;
}

void protocol(UART_t uart) {
    // Protocol, Baud Rate, and Interrupt Settings
    // 1
    *uart.MDR1 = 0x7;
    // 2
    *uart.LCR = 0x00BF;
    // 3
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
    // 4
    *uart.LCR = 0x0;
    // 5
    *uart.IER = 0x0;
    // 6
    *uart.LCR = 0x00BF;
    // 7
    *uart.DLL = 0x1A;
    *uart.DLH = 0x0;
    // 8
    *uart.LCR = 0x0;
    // 9 no interrupts
    // 10
    *uart.LCR = 0x00BF;
    // 11
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 12
    clearBit(uart.LCR, LCR_DIV_EN);
    clearBit(uart.LCR, LCR_BREAK_EN);

    clearBit(uart.LCR, LCR_PARITY_EN);

    setBit(uart.LCR, LCR_CHAR_LENGTH_1);
    setBit(uart.LCR, LCR_CHAR_LENGTH_2);

    clearBit(uart.LCR, LCR_NB_STOP);
    // 13
    clearBit(uart.MDR1, MDR1_MODE_SELECT_1);
    clearBit(uart.MDR1, MDR1_MODE_SELECT_2);
    clearBit(uart.MDR1, MDR1_MODE_SELECT_3);
}

void hwFlowControl(UART_t uart) {
    // Hardware and Software Flow Control Configuration
    // 1
    uint8_t savedLcr = *uart.LCR;
    *uart.LCR = 0x0080;
    // 2
    uint8_t savedTcrTlr = getBit(uart.MCR, MCR_TCR_TLR);
    setBit(uart.MCR, MCR_TCR_TLR);
    // 3
    *uart.LCR = 0x00BF;
    // 4
    uint8_t savedEnhancedEn = getBit(uart.EFR, EFR_ENHANCED_EN);
    setBit(uart.EFR, EFR_ENHANCED_EN);
    // 5
    clearBit(uart.TCR, 0);
    clearBit(uart.TCR, 1);
    clearBit(uart.TCR, 2);
    clearBit(uart.TCR, 3);
    setBit(uart.TCR, 4);
    setBit(uart.TCR, 5);
    setBit(uart.TCR, 6);
    setBit(uart.TCR, 7);
    // 6
    setBit(uart.EFR, EFR_AUTO_CTS_EN);
    setBit(uart.EFR, EFR_AUTO_RTS_EN);
    restoreBit(uart.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 7
    *uart.LCR = 0x0080;
    // 8
    restoreBit(uart.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 9
    *uart.LCR = savedLcr;
}

UART_t createUart(uint32_t baseAddress) {
    UART_t uart = {
        .MDR1   = (uint8_t*) (baseAddress + MDR1_OFF),
        .LCR    = (uint8_t*) (baseAddress + LCR_OFF),
        .EFR    = (uint8_t*) (baseAddress + EBLR_OFF),
        .DLL    = (uint8_t*) (baseAddress + DLL_OFF),
        .DLH    = (uint8_t*) (baseAddress + DLH_OFF),
        .MCR    = (uint8_t*) (baseAddress + MCR_OFF),
        .IER    = (uint8_t*) (baseAddress + IER_OFF),
        .TXFLL  = (uint8_t*) (baseAddress + TXFLL_OFF),
        .EBLR   = (uint8_t*) (baseAddress + EBLR_OFF),
        .ACREG  = (uint8_t*) (baseAddress + ACREG_OFF),
        .THR    = (uint8_t*) (baseAddress + THR_OFF),
        .LSR    = (uint8_t*) (baseAddress + LSR_OFF),
        .SYSC   = (uint8_t*) (baseAddress + SYSC_OFF),
        .SYSS   = (uint8_t*) (baseAddress + SYSS_OFF),
        .FCR    = (uint8_t*) (baseAddress + FCR_OFF),
        .TLR    = (uint8_t*) (baseAddress + TLR_OFF),
        .SCR    = (uint8_t*) (baseAddress + SCR_OFF),
        .TCR    = (uint8_t*) (baseAddress + TCR_OFF),
        .RHR    = (uint8_t*) (baseAddress + RHR_OFF)
    };
    return uart;
}

int uart_main(void) {
    UART_t uart3 = createUart(UART3_BASE);

    swReset(uart3);
    fifoAndDmaSettings(uart3);
    protocol(uart3);
    hwFlowControl(uart3);

    char hi[] = "Hi";
    int i = 0;
    while (hi[i]) {
        while (!(getBit(uart3.LSR, LSR_TX_FIFO_E)));
        *uart3.THR = hi[i];
        i++;
    }

    while (1) {
        while (!getBit(uart3.LSR, LSR_RX_FIFO_E));
        char in = *uart3.RHR;
        while (!(getBit(uart3.LSR, LSR_TX_FIFO_E)));
        *uart3.THR = in;
    }

    return 0;
}
