/*
 * queue.c
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */
#include "global/types.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue_t queue_create(void){
    Queue_t queue = {.pFront = NULL, .pRear = NULL, .size = 0};
    return queue;
}

void queue_insert(Queue_t * queue, void * pNewEntry)
{
    queue->size++;
    if (queue->pFront == NULL && queue->pRear == NULL)
    {
        queue->pFront = queue->pRear = pNewEntry;
        return;
    }
    queue->pRear->next = pNewEntry;
    queue->pRear = pNewEntry;
}

void queue_remove(Queue_t * queue)
{
    struct Node* temp = queue->pFront;
    if (queue->pFront == NULL)
    {
        printf("Queue is Empty\n");
        return;
    }
    if (queue->pFront == queue->pRear)
    {
        queue->pFront = queue->pRear = NULL;
    }
    else
    {
        queue->pFront = queue->pFront->next;
    }

    queue->size--;
    free(temp);
}

void * queue_front(Queue_t * queue){
    return queue->pFront;
}

void * queue_rear(Queue_t * queue){
    return queue->pRear;
}

uint8_t queue_isEmpty(Queue_t * queue)
{
    if (queue->pFront == NULL)
    {
        return TRUE;
    }

    return FALSE;
}
