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

#define INTERRUPT_BASE      0x4820000

#define NROF_IR_VECTORS     96

#define INTCPS_REVISION     INTERRUPT_BASE + 0x0000  // R
#define INTCPS_SYSCONFIG    INTERRUPT_BASE + 0x0010  // RW
#define INTCPS_SYSSTATUS    INTERRUPT_BASE + 0x0014  // R
#define INTCPS_SIR_IRQ      INTERRUPT_BASE + 0x0040  // R
#define INTCPS_SIR_FIQ      INTERRUPT_BASE + 0x0044  // R
#define INTCPS_CONTROL      INTERRUPT_BASE + 0x0048  // RW
#define INTCPS_PROTECTION   INTERRUPT_BASE + 0x004C  // RW
#define INTCPS_IDLE         INTERRUPT_BASE + 0x0050  // RW
#define INTCPS_IRQ_PRIORITY INTERRUPT_BASE + 0x0060  // RW
#define INTCPS_FIQ_PRIORITY INTERRUPT_BASE + 0x0064  // RW
#define INTCPS_THRESHOLD    INTERRUPT_BASE + 0x0068  // RW

/* Macros to manipulate interrupt registers */
// n = 0 to 2
// m = 0 to 95
#define INTCPS_ITR(n)       INTERRUPT_BASE + 0x0080 + (0x20 * n)
#define INTCPS_MIR(n)       INTERRUPT_BASE + 0x0084 + (0x20 * n)
#define INTCPS_MIR_CLEAR(n) INTERRUPT_BASE + 0x0088 + (0x20 * n)
#define INTCPS_MIR_SET(n)   INTERRUPT_BASE + 0x008C + (0x20 * n)
#define INTCPS_ISR_SET(n)   INTERRUPT_BASE + 0x0090 + (0x20 * n)
#define INTCPS_ISR_CLEAR(n) INTERRUPT_BASE + 0x0094 + (0x20 * n)
#define INTCPS_PENDING_IRQ(n)   INTERRUPT_BASE + 0x0098 + (0x20 * n)
#define INTCPS_PENDING_FIQ(n)   INTERRUPT_BASE + 0x009C + (0x20 * n)
#define INTCPS_ILR(m)       INTERRUPT_BASE + 0x0100 + (0x4 * m)

/* Further definitions */
#define INTCPS_ILR_FIQ            BIT0
#define INTCPS_SIR_IRQ_MASK       (0x7F) // to get the bitfild (check page 1060)
#define INTCPS_CONTROL_NEWIRQAGR  BIT0
#define INTCPS_CONTROL_NEWFIQAGR  BIT1

#define ACTIVE_IRQ_NUM  ((*(uint_32*)INTCPS_SIR_IRQ) &INTCPS_SIR_IRQ_MASK) // gets the active IRQ number

extern void enable_interrupts();
extern void disable_interrupts();

typedef void (*InterruptHandler_t) (void);

void init_irq(void);
void register_interrupt_handler(InterruptHandler_t handler, uint8_t irq_nr);
static void dispatch_interrupts(void);

extern uint8_t * get_pending_irqs(void);
extern void enable_irq(uint8_t irq_nr);

#endif /* INCLUDES_INTERRUPTS_H_ */
