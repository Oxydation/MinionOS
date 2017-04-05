/*
 * SysCallLib.h
 *
 *  Created on: 05.04.2017
 *      Author: Jasmin
 */

#ifndef APPLICATIONS_SYSCALLLIB_H_
#define APPLICATIONS_SYSCALLLIB_H_
#include <stdbool.h>
/*LEDs steuern
* turnOn = true=HIGH oder false=LOW
* led = 0 (für usr0 LED) oder 1 (für usr1 LED)
**/
#pragma SWI_ALIAS(control_led, 1);
int control_led(bool turnOn, int led);


#endif /* APPLICATIONS_SYSCALLLIB_H_ */
