/*
 * Omap3530Interrupts.c
 *
 *  Created on: 18 Mar 2017
 *      Author: Mathias
 *  Description: Device dependent implementation of interrupt stuff.
 */
#include "includes/Interrupts.h"
#include <string.h>
#include "includes/Common.h"
#include "includes/Omap3530.h"

uint8_t * get_pending_irqs(void){
    uint8_t * pendingIrqs;
    memset(pendingIrqs, 0, NROF_IR_VECTORS * sizeof(uint8_t));

    int i = 0;
    for(i = 0; i < NROF_IR_VECTORS; i++){
        if(get_32(INTCPS_PENDING_IRQ()) & (1u <<i)){
            pendingIrqs[i] = TRUE;
        }
    }

    return pendingIrqs;
}

void enable_irq(uint8_t irq_nr){
    set_32(INTCPS_MIR_SET(irq_nr/REGISTER_SIZE), irq_nr % REGISTER_SIZE);
}
