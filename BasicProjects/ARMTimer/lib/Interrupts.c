/*
 * Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 */
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

/*
 * Dispatches the interrupts, if an interrupt is pending.
 * If a handler for the given IRQ is set, then the handler will be called.
 */
#pragma INTERRUPT(dispatch_interrupts, IRQ);
#pragma RETAIN(dispatch_interrupts);
interrupt void dispatch_interrupts(void) //
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

