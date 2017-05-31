/*
 * queue.h
 *
 *  Created on: 17 Apr 2017
 *      Author: Mathias
 */

#ifndef GLOBAL_QUEUE_QUEUE_H_
#define GLOBAL_QUEUE_QUEUE_H_

#include <inttypes.h>

typedef struct Node
{
    void * data;
    struct Node * next;
} Node_t;

typedef struct Queue
{
    Node_t * pFront;
    Node_t * pRear;
    uint16_t size;
} Queue_t;

Queue_t queue_create(void);
void queue_insert(Queue_t * queue, void * data);
void queue_remove(Queue_t * queue);
uint8_t queue_isEmpty(Queue_t * queue);
void * queue_front(Queue_t*  queue);

#endif /* GLOBAL_QUEUE_QUEUE_H_ */
