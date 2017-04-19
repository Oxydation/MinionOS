/*
 * interrupts.h
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Definitions used for interrupt functionality, based on TRM Page 1068
 */

#ifndef INCLUDES_INTERRUPTS_H_
#define INCLUDES_INTERRUPTS_H_

#include <inttypes.h>

extern void enable_interrupts();
extern void disable_interrupts();

typedef void (*InterruptHandler_t)(void);

void init_irq(void);
void register_interrupt_handler(InterruptHandler_t handler, uint8_t irq_nr);
extern void get_pending_irqs(uint8_t * pendingIrqs);
extern void enable_irq_source(uint8_t irq_source);
extern void disable_irq_source(uint8_t irq_source);
void disable_all_interrupt_sources();
uint8_t get_irq_source_state(uint8_t irq_source);
void dispatch_interrupts(void);

#endif /* INCLUDES_INTERRUPTS_H_ */
