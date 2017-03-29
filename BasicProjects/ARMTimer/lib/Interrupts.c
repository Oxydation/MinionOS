/*
 * Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 */

#include <stdio.h>
#include "includes/Interrupts.h"
#include "includes/Common.h"

// Keep book of all interrupt handlers
static InterruptHandler_t gInterruptHandlers[NROF_IR_VECTORS] = { 0 };

/*
 * Registers a new interrupt handler at a given IRQ-Position.
 */
void register_interrupt_handler(InterruptHandler_t handler, uint8_t irq_nr)
{
    enable_irq_source(irq_nr);
    gInterruptHandlers[irq_nr] = handler; // set handler at given irq
}

#pragma SET_CODE_SECTION(".ISR")
#pragma INTERRUPT (isr_irq, IRQ)
void isr_irq(void)
{
    // Disable futher interrupts
    unsigned int interruptsState = _disable_interrupts();
    _disable_IRQ();
    unsigned int dst = _get_CPSR();

    // Call the dispatcher of the interrupts
   // dispatch_interrupts();

    // TODO: Just for test!!
    InterruptHandler_t handler = gInterruptHandlers[38];
   if (handler != 0)
      handler(); // call handler if set

    _restore_interrupts(interruptsState);

    __asm(" SUBS PC,R14,#4;");

}

/*
 * Dispatches the interrupts, if an interrupt is pending.
 * If a handler for the given IRQ is set, then the handler will be called.
 */

void dispatch_interrupts(void) //
{
    uint8_t pendingIrqs[NROF_IR_VECTORS];
    // get pending irqs and store in pendingIrq bool array

    get_pending_irqs(pendingIrqs);
    int i = 0;
    for (i = 0; i < NROF_IR_VECTORS; i++)
    {
        if (pendingIrqs[i])
        {
            InterruptHandler_t handler = gInterruptHandlers[i];
            if (handler != 0)
                handler(); // call handler if set
        }
    }
}

