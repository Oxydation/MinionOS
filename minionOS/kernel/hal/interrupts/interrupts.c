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
#include "systemCallArguments.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"
#include "kernel/systemModules/mmu/mmu.h"
#include "kernel/systemModules/scheduler/scheduler.h"
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
    int swi = 0;                            // Initialize variable to shut up compiler warning
    __asm(" LDR r12, [lr,#-4]");            // load SWI instruction into R12
    __asm(" BIC r12, r12, #0xff000000");    // apply bit mask to R12 to only include SWI number
    __asm(" STR r12, [sp]");                // store R12 in swi
    if (swi == SYSTEM_CALL_SWI_NUMBER) {
        return dispatcher_dispatch(args);
    } else if (swi == SWITCH_TO_IDLE_SWI_NUMBER) {
        scheduler_switchToIdleProcess(&g_pcb);
        return 0;
    } else {
        return -1;
    }
}

#pragma INTERRUPT (isr_fiq, FIQ)
void isr_fiq(void) {

}

#pragma INTERRUPT (isr_undef, UDEF)
void isr_undef(void) {

}
#pragma INTERRUPT (isr_undef, DABT)
void isr_dabt(void) {

    uint8_t faultStatus = mmu_getDataFaultStatus();
    uint32_t faultAddress = mmu_getDataFaultAddress();

    if (faultAddress == NULLPOINTER)
    {
        processManager_terminateCurrentProcess(&g_pcb);
    }
    else if (faultStatus == TRANSLATION_FAULT_SECTION)
    {
        mmu_handleSectionTranslationFault(faultAddress);
    }

    asm_loadContext(&g_pcb);
}

#pragma INTERRUPT (isr_undef, PABT)
void isr_pabt(void) {

    uint8_t faultStatus = mmu_getInstructionFaultStatus();
    uint32_t faultAddress = mmu_getInstructionFaultAddress();

    if (faultAddress == NULLPOINTER)
    {
       processManager_terminateCurrentProcess(&g_pcb);
    }
    else if (faultStatus == TRANSLATION_FAULT_SECTION)
    {
        mmu_handleSectionTranslationFault(faultAddress);
    }

    asm_loadContext(&g_pcb);
}
