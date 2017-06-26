#ifndef KERNEL_DEVICES_OMAP3530_INCLUDES_UART_H_
#define KERNEL_DEVICES_OMAP3530_INCLUDES_UART_H_


// Base addresses
#define UART1_BASE       (0x4806A000)
#define UART2_BASE       (0x4806C000)
#define UART3_BASE       (0x49020000)

#define UART_OPERATING_FREQUENCY    (48000000)

// Register offsets
#define DLL_OFF     (0x0)
#define DLH_OFF     (0x4)
#define MDR1_OFF    (0x20)
#define LCR_OFF     (0xC)
#define EFR_OFF     (0x8)
#define MCR_OFF     (0x10)
#define IER_OFF     (0x4)
#define TXFLL_OFF   (0x28)
#define EBLR_OFF    (0x48)
#define ACREG_OFF   (0x3C)
#define THR_OFF     (0x0)
#define LSR_OFF     (0x14)
#define SYSC_OFF    (0x54)
#define SYSS_OFF    (0x58)
#define FCR_OFF     (0x8)
#define TLR_OFF     (0x01)
#define SCR_OFF     (0x40)
#define TCR_OFF     (0x18)
#define RHR_OFF     (0x0)
#define IIR_OFF     (0x08)

// Indices for register fields
#define EFR_ENHANCED_EN     (4)
#define EFR_AUTO_CTS_EN     (7)
#define EFR_AUTO_RTS_EN     (6)

#define MCR_TCR_TLR         (6)

#define FCR_RX_FIFO_TRIG_1  (6)
#define FCR_RX_FIFO_TRIG_2  (7)
#define FCR_TX_FIFO_TRIG_1  (4)
#define FCR_TX_FIFO_TRIG_2  (5)
#define FCR_DMA_MODE        (3)
#define FCR_FIFO_ENABLE     (0)

#define SCR_TX_TRIG_GRANU1  (6)
#define SCR_DMA_MODE_2_1    (1)

#define LCR_PARITY_EN       (3)
#define LCR_PARITY_TYPE2    (5)
#define LCR_PARITY_TYPE1    (4)
#define LCR_DIV_EN          (7)
#define LCR_BREAK_EN        (6)
#define LCR_CHAR_LENGTH_1   (0)
#define LCR_CHAR_LENGTH_2   (1)
#define LCR_NB_STOP         (2)

#define MDR1_MODE_SELECT_1  (0)
#define MDR1_MODE_SELECT_2  (1)
#define MDR1_MODE_SELECT_3  (2)

#define LSR_TX_FIFO_E       (5)
#define LSR_RX_FIFO_E       (0)

#endif /* KERNEL_DEVICES_OMAP3530_INCLUDES_UART_H_ */
