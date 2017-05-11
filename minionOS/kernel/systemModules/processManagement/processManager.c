/*
 * processManager.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#include "../scheduler/scheduler.h"
#include "processManager.h"

void processManager_loadProcess(uint32_t startAddress, uint32_t stackPointer){
    scheduler_startProcess(startAddress, stackPointer, 0x60000110);
}

void processManager_startFirstProcess(void){

}
