/*
 * Interrupts.h
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Definitions used for interrupt functionality, based on TRM Page 1068
 */

#ifndef INCLUDES_INTERRUPTS_H_
#define INCLUDES_INTERRUPTS_H_

#include <inttypes.h>

#define INTERRUPT_BASE      0x48200000

#define NROF_IR_VECTORS     96

#define INTCPS_REVISION     (INTERRUPT_BASE + 0x0000)  // R
#define INTCPS_SYSCONFIG    (INTERRUPT_BASE + 0x0010)  // RW
#define INTCPS_SYSSTATUS    (INTERRUPT_BASE + 0x0014)  // R
#define INTCPS_SIR_IRQ      (INTERRUPT_BASE + 0x0040)  // R
#define INTCPS_SIR_FIQ      (INTERRUPT_BASE + 0x0044)  // R
#define INTCPS_CONTROL      (INTERRUPT_BASE + 0x0048)  // RW
#define INTCPS_PROTECTION   (INTERRUPT_BASE + 0x004C)  // RW
#define INTCPS_IDLE         (INTERRUPT_BASE + 0x0050)  // RW
#define INTCPS_IRQ_PRIORITY (INTERRUPT_BASE + 0x0060)  // RW
#define INTCPS_FIQ_PRIORITY (INTERRUPT_BASE + 0x0064)  // RW
#define INTCPS_THRESHOLD    (INTERRUPT_BASE + 0x0068)  // RW

/* Macros to manipulate interrupt registers */
// n = 0 to 2
// m = 0 to 95
#define INTCPS_ITR(n)       (INTERRUPT_BASE + 0x0080 + (0x20 * n))
#define INTCPS_MIR(n)       (INTERRUPT_BASE + 0x0084 + (0x20 * n))
#define INTCPS_MIR_CLEAR(n) (INTERRUPT_BASE + 0x0088 + (0x20 * n))
#define INTCPS_MIR_SET(n)   (INTERRUPT_BASE + 0x008C + (0x20 * n))
#define INTCPS_ISR_SET(n)   (INTERRUPT_BASE + 0x0090 + (0x20 * n))
#define INTCPS_ISR_CLEAR(n) (INTERRUPT_BASE + 0x0094 + (0x20 * n))
#define INTCPS_PENDING_IRQ(n)   (INTERRUPT_BASE + 0x0098 + (0x20 * n))
#define INTCPS_PENDING_FIQ(n)   (INTERRUPT_BASE + 0x009C + (0x20 * n))
#define INTCPS_ILR(m)       (INTERRUPT_BASE + 0x0100 + (0x4 * m))

/* Further definitions */
#define INTCPS_ILR_FIQ              (1<<0)
#define INTCPS_SIR_IRQ_MASK         (0x7F) // to get the bitfild (check page 1060)
#define INTCPS_CONTROL_NEWIRQAGR    (1<<0)
#define INTCPS_CONTROL_NEWFIQAGR    (1<<1)

#define ACTIVE_IRQ_NUM  ((*(uint32_t*)INTCPS_SIR_IRQ) &INTCPS_SIR_IRQ_MASK) // gets the active IRQ number

extern void enable_interrupts();
extern void disable_interrupts();

typedef void (*InterruptHandler_t)(void);

void init_irq(void);
void register_interrupt_handler(InterruptHandler_t handler, uint8_t irq_nr);
//__irq void dispatch_interrupts(void);

extern void get_pending_irqs(uint8_t * pendingIrqs);
extern void enable_irq_source(uint8_t irq_source);
extern void disable_irq_source(uint8_t irq_source);
void disable_all_interrupt_sources();
uint8_t get_irq_source_state(uint8_t irq_source);
void dispatch_interrupts(void);

/* IRQ Sources, check Page 1053 */
/*      Source      M_IRQ_x     Description*/
#define SDMA_IRQ_0  12  // System DMA request 0
#define SDMA_IRQ_1  13  // System DMA request 1
#define SDMA_IRQ_2  14  // System DMA request 2
#define SDMA_IRQ_3  15  // System DMA request 3

#define GPMC_IRQ    20  // General-purpose memory controller module

#define GPIO1_MPU_IRQ   29 //  GPIO module 1
#define GPIO2_MPU_IRQ   30 //  GPIO module 2
#define GPIO3_MPU_IRQ   31 //  GPIO module 3
#define GPIO4_MPU_IRQ   32 //  GPIO module 4
#define GPIO5_MPU_IRQ   33 //  GPIO module 5
#define GPIO6_MPU_IRQ   34 //  GPIO module 6

#define GPT1_IRQ    37  // General-purpose Timer 1
#define GPT2_IRQ    38
#define GPT3_IRQ    39
#define GPT4_IRQ    40
#define GPT5_IRQ    41
#define GPT6_IRQ    42
#define GPT7_IRQ    43
#define GPT8_IRQ    44
#define GPT9_IRQ    45
#define GPT10_IRQ   46
#define GPT11_IRQ   47

#define I2C1_IRQ    56  // I2C Module 1
#define I2C2_IRQ    57  // I2C Module 2
#define I2C3_IRQ    61  // I2C Module 3

#define SPI1_IRQ    65  // Mc SPI module 1
#define SPI2_IRQ    66  // Mc SPI module 2
#define SPI3_IRQ    91  // MC SPI module 3

#define UART1_IRQ   72  // UART Module 1
#define UART2_IRQ   73  // UART Module 2
#define UART3_IRQ   74  // UART Module 3

#define OHCI_IRQ    76  // OHCI Controller HSUSB MP Host Interrupt
#define EHCI_IRQ    77  // EHCI controller HSUSB MP Host Interrupt

#define MMC1_IRQ    83  // MMC/SD Module 1
#define MMC2_IRQ    86  // MMC/SD Module 2

#define HSUSB_MC_NINT   92  // High-Speed USB OTG controller
#define HSUSB_DMA_NINT  93  // High-Speed USB OTG DMA controller

#define MMC3_IRQ    94  // MMC/SD Module 3

#endif /* INCLUDES_INTERRUPTS_H_ */
