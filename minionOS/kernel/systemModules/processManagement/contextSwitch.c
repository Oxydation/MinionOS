/*
 * contextSwitch.c
 *
 *  Created on: 11 May 2017
 *      Author: Mathias
 */

#include "contextSwitch.h"

void copyPcb(PCB_t * source, PCB_t * target){
    target->lr = source->lr;
    target->cpsr = source->cpsr;
    target->registers = source->registers;
}
