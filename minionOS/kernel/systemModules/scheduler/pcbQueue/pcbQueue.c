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

void pcbQueue_removePcbNode(Queue_t* queue, PCB_t* pcb) {

    PcbNode_t* previousNode;
    PcbNode_t* currNode = (PcbNode_t*)queue->pFront;

    while (currNode != NULL) {
        if (currNode->data->processId == pcb->processId) {
            if (previousNode != NULL) {
                previousNode->pNext = currNode->pNext;
            } else {
                queue->pFront = currNode->pNext;
            }
            break;
        }
    }
    queue->size--;
}
