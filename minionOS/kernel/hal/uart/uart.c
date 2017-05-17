#include "uart.h"
#include <inttypes.h>

#define BASE1       0x4806A000
#define BASE2       0x4806C000
#define BASE3       0x49020000
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

int uart_main(void) {
    UART_t uart3 = {
             .MDR1 = (uint8_t*) (BASE3 + MDR1_OFF),
             .LCR = (uint8_t*) (BASE3 + LCR_OFF),
             .EFR = (uint8_t*) (BASE3 + EBLR_OFF),
             .DLL = (uint8_t*) (BASE3 + DLL_OFF),
             .DLH = (uint8_t*) (BASE3 + DLH_OFF),
             .MCR = (uint8_t*) (BASE3 + MCR_OFF),
             .IER = (uint8_t*) (BASE3 + IER_OFF),
             .TXFLL = (uint8_t*) (BASE3 + TXFLL_OFF),
             .EBLR = (uint8_t*) (BASE3 + EBLR_OFF),
             .ACREG = (uint8_t*) (BASE3 + ACREG_OFF),
             .THR = (uint8_t*) (BASE3 + THR_OFF),
             .LSR = (uint8_t*) (BASE3 + LSR_OFF),
             .SYSC = (uint8_t*) (BASE3 + SYSC_OFF),
             .SYSS = (uint8_t*) (BASE3 + SYSS_OFF),
             .FCR = (uint8_t*) (BASE3 + FCR_OFF),
             .TLR = (uint8_t*) (BASE3 + TLR_OFF),
             .SCR = (uint8_t*) (BASE3 + SCR_OFF),
             .TCR = (uint8_t*) (BASE3 + TCR_OFF),
             .RHR = (uint8_t*) (BASE3 + RHR_OFF)
    };

    // Software Reset
    // 1
    setBit(uart3.SYSC, 1);
    // 2
    while (!getBit(uart3.SYSS, 0));

    // FIFOs and DMA Settings
    // 1
    uint8_t savedLcr = *uart3.LCR;
    *uart3.LCR = 0x00BF;
    // 2
    uint8_t savedEnhancedEn = getBit(uart3.EFR, EFR_ENHANCED_EN);
    setBit(uart3.EFR, EFR_ENHANCED_EN);
    // 3
    *uart3.LCR = 0x0080;
    // 4
    uint8_t savedTcrTlr = getBit(uart3.MCR, MCR_TCR_TLR);
    setBit(uart3.MCR, MCR_TCR_TLR);
    // 5
    clearBit(uart3.FCR, FCR_RX_FIFO_TRIG_1);
    clearBit(uart3.FCR, FCR_RX_FIFO_TRIG_2);

    clearBit(uart3.FCR, FCR_TX_FIFO_TRIG_1);
    clearBit(uart3.FCR, FCR_TX_FIFO_TRIG_2);

    clearBit(uart3.FCR, FCR_DMA_MODE);

    setBit(uart3.FCR, FCR_FIFO_ENABLE);
    // 6
    *uart3.LCR = 0x00BF;
    // 7
    *uart3.TLR = 0x0000;
    // 8
    setBit(uart3.SCR, SCR_TX_TRIG_GRANU1);
    setBit(uart3.SCR, SCR_DMA_MODE_2_1);
    // 9
    restoreBit(uart3.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 10
    *uart3.LCR = 0x0080;
    // 11
    restoreBit(uart3.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 12
    *uart3.LCR = savedLcr;

    // Protocol, Baud Rate, and Interrupt Settings
    // 1
    *uart3.MDR1 = 0x7;
    // 2
    *uart3.LCR = 0x00BF;
    // 3
    savedEnhancedEn = getBit(uart3.EFR, EFR_ENHANCED_EN);
    setBit(uart3.EFR, EFR_ENHANCED_EN);
    // 4
    *uart3.LCR = 0x0;
    // 5
    *uart3.IER = 0x0;
    // 6
    *uart3.LCR = 0x00BF;
    // 7
    *uart3.DLL = 0x1A;
    *uart3.DLH = 0x0;
    // 8
    *uart3.LCR = 0x0;
    // 9 no interrupts
    // 10
    *uart3.LCR = 0x00BF;
    // 11
    restoreBit(uart3.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 12
    clearBit(uart3.LCR, LCR_DIV_EN);
    clearBit(uart3.LCR, LCR_BREAK_EN);

    clearBit(uart3.LCR, LCR_PARITY_EN);

    setBit(uart3.LCR, LCR_CHAR_LENGTH_1);
    setBit(uart3.LCR, LCR_CHAR_LENGTH_2);

    clearBit(uart3.LCR, LCR_NB_STOP);
    // 13
    clearBit(uart3.MDR1, MDR1_MODE_SELECT_1);
    clearBit(uart3.MDR1, MDR1_MODE_SELECT_2);
    clearBit(uart3.MDR1, MDR1_MODE_SELECT_3);

    // Hardware and Software Flow Control Configuration
    // 1
    savedLcr = *uart3.LCR;
    *uart3.LCR = 0x0080;
    // 2
    savedTcrTlr = getBit(uart3.MCR, MCR_TCR_TLR);
    setBit(uart3.MCR, MCR_TCR_TLR);
    // 3
    *uart3.LCR = 0x00BF;
    // 4
    savedEnhancedEn = getBit(uart3.EFR, EFR_ENHANCED_EN);
    setBit(uart3.EFR, EFR_ENHANCED_EN);
    // 5
    clearBit(uart3.TCR, 0);
    clearBit(uart3.TCR, 1);
    clearBit(uart3.TCR, 2);
    clearBit(uart3.TCR, 3);
    setBit(uart3.TCR, 4);
    setBit(uart3.TCR, 5);
    setBit(uart3.TCR, 6);
    setBit(uart3.TCR, 7);
    // 6
    setBit(uart3.EFR, EFR_AUTO_CTS_EN);
    setBit(uart3.EFR, EFR_AUTO_RTS_EN);
    restoreBit(uart3.EFR, EFR_ENHANCED_EN, savedEnhancedEn);
    // 7
    *uart3.LCR = 0x0080;
    // 8
    restoreBit(uart3.MCR, MCR_TCR_TLR, savedTcrTlr);
    // 9
    *uart3.LCR = savedLcr;

    char hi[] = "Hi";
    int i = 0;
    while (hi[i]) {
        while (!(getBit(uart3.LSR, LSR_TX_FIFO_E)));
        *uart3.THR = hi[i];
        i++;
    }

    while (1) {
        while (getBit(uart3.LSR, LSR_RX_FIFO_E));
        char in = *uart3.RHR;
        while (!(getBit(uart3.LSR, LSR_TX_FIFO_E)));
        *uart3.THR = in;
    }

    return 0;
}
