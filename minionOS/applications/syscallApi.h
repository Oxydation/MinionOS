/*
 * syscalls.h
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */

#ifndef APPLICATIONS_SYSCALLAPI_H_
#define APPLICATIONS_SYSCALLAPI_H_
#include <stdbool.h>
#define LED_0   0
#define LED_1   1

int sysCalls_enableLed(bool turnOn, int led);

#endif /* APPLICATIONS_SYSCALLAPI_H_ */
