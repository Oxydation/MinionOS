/*
 * scheduler.h
 *
 *  Created on: 13 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_
#define KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_

#define MAX_ALLOWED_PROCESSES 16

void scheduler_init(void);
void scheduler_start(void);
void scheduler_stop(void);

void scheduler_startProcess();
void scheduler_stopProcess();

#endif /* KERNEL_SYSTEMMODULES_SCHEDULER_SCHEDULER_H_ */
