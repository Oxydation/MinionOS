/*
 * Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Provides common interrupt initialization and handling.
 *  Important information: The stack must be set up to handle interrupts.
 */

#include "kernel/common/mmio.h"
#include "kernel/devices/omap3530/includes/interrupts.h"
#include "kernel/hal/interrupts/interrupts.h"
#include "kernel/systemModules/systemCalls/dispatcher.h"
#include "kernel/systemModules/systemCalls/systemCallArguments.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"
#include "global/types.h"
#include <stdio.h>

// Keep book of all interrupt handlers
static InterruptHandler_t g_interruptHandlers[NROF_IR_VECTORS] = { 0 };
static PCB_t g_pcb;
static volatile uint32_t g_lrError = 0;

/*
 * Registers a new interrupt handler at a given IRQ-Position.
 */
void interrupts_registerHandler(InterruptHandler_t handler, uint8_t irq_nr) {
    interrupts_enableIrqSource(irq_nr);
    g_interruptHandlers[irq_nr] = handler; // set handler at given irq
}

void enable_interrupts() {
    _enable_interrupts();
}

void disable_interrupts() {
    _disable_interrupts();
}

#pragma SET_CODE_SECTION(".ISR")
#pragma INTERRUPT (isr_irq, IRQ)
void isr_irq(void) {

    // 1. Step: Remember LR (automatically done by compiler, including R0-R3 & R12)
    __asm(" ADD SP, SP, #8 ");

    // 2. Step: Save previous context into PCB
    asm_saveContext(&g_pcb);

    // 3. Step: Handle Interrupts and reactivate
    uint32_t activeIrq = (*(Address_t) (INTCPS_SIR_IRQ)) & INTCPS_SIR_IRQ_MASK;
    InterruptHandler_t handler = g_interruptHandlers[activeIrq];
    if (handler != 0)
        handler(activeIrq, &g_pcb);

    // Clear IRQ interrupt output
    (*(Address_t) (INTCPS_CONTROL)) |= INTCPS_CONTROL_NEWIRQAGR;

    asm_dataSynchronizationBarrier();

    // 4. Step: Restore registers (R0-R4 & R1 are automatically restored by compiler)
    // 5. Step: Restore LR (automatically by compiler)

    // Correct SP to be the same after the interrupt
    __asm(" ADD SP, SP, #4 ");

    // 6. Step Continuing with last user process (auto done)
    asm_loadContext(&g_pcb);
}

#pragma INTERRUPT (isr_reset, RESET)
void isr_reset(void) {

}

#pragma INTERRUPT (isr_swi, SWI)
int isr_swi(SysCallArgs_t args) {
    return dispatcher_dispatch(args);
}

#pragma INTERRUPT (isr_fiq, FIQ)
void isr_fiq(void) {

}

#pragma INTERRUPT (isr_undef, UDEF)
void isr_undef(void) {

}
#pragma INTERRUPT (isr_undef, DABT)
void isr_dabt(void) {

}

#pragma INTERRUPT (isr_undef, PABT)
void isr_pabt(void) {

}
