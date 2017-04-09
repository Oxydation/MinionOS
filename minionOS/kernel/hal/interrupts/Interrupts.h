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

extern void enable_interrupts();
extern void disable_interrupts();

typedef void (*InterruptHandler_t)(uint32_t);

void interrupts_initIrq(void);
void interrupts_registerHandler(InterruptHandler_t handler, uint8_t irq_nr);

extern void interrupts_enableIrqSource(uint8_t irq_source);
extern void interrupts_disableIrqSource(uint8_t irq_source);
extern void interrupts_disableAllInterruptSources();
extern void interrupts_getPendingIrqs(uint8_t * pendingIrqs);
extern uint8_t interrupts_getIrqSourceState(uint8_t irq_source);


#endif /* INCLUDES_INTERRUPTS_H_ */
