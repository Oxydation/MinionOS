/*
 * pcbQueue.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */
#include <stdio.h>
#include <stdlib.h>
#include "pcbQueue.h"

PcbNode_t* pcbQueue_createPcbNode(PCB_t * pcb){
    PcbNode_t* newNode = malloc(sizeof(PcbNode_t));
        if (newNode != NULL) {
            newNode->data = pcb;
            newNode->pNext = NULL;
        }
        return newNode;
}
