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
    // Disable further interrupts
    //_disable_interrupts();
    //_disable_IRQ();
    //unsigned int dst = _get_CPSR();

    // Call the dispatcher of the interrupts
    // dispatch_interrupts();

    uint32_t activeIRQ = get_32(ACTIVE_IRQ_NUM);
    if(activeIRQ == 38){

        InterruptHandler_t handler = gInterruptHandlers[38];
         if (handler != 0)
            handler(); // call handler if set
    }

    //_restore_interrupts(interruptsState);
   // _enable_interrupts();
   // _enable_IRQ();

    //__asm(" SUBS PC,R14,#4;");



}

#pragma INTERRUPT (isr_reset, RESET)
void isr_reset(void)
{

}

#pragma INTERRUPT (isr_swii, SWI)
void isr_swii(void)
{
    __asm(" MOVS PC,R14;");
}

#pragma INTERRUPT (isr_fiq, FIQ)
void isr_fiq(void){

}

#pragma INTERRUPT (isr_undef, UDEF)
void isr_undef(void){

}
#pragma INTERRUPT (isr_undef, DABT)
void isr_dabt(void){

}

#pragma INTERRUPT (isr_undef, PABT)
void isr_pabt(void){

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

