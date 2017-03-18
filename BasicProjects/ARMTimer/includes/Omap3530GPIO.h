/*
 * Omap3530GPIO.h
 *
 *  Created on: 15 Mar 2017
 *      Author: Mathias
 *  Description: Various defines used for the OMAP 3530, GPIO-Related.
 */

#ifndef OMAP3530GPIO_H_
#define OMAP3530GPIO_H_

#include <inttypes.h>

#define INPUT 0x1
#define OUTPUT 0x0


/* Offset address for GPIO-Modules 1 to 6 */
#define GPIO1_BASE (0x48310000)
#define GPIO2_BASE (0x49050000)
#define GPIO3_BASE (0x49052000)
#define GPIO4_BASE (0x49054000)
#define GPIO5_BASE (0x49056000)
#define GPIO6_BASE (0x49058000)

/* Various GPIO Registers */
#define GPIO_DATAIN           (0x0038)
#define GPIO_DATAOUT          (0x003C)
#define GPIO_CTRL             (0x0030)
#define GPIO_OE               (0x0034)
#define GPIO_CLEARDATAOUT     (0x0090)
#define GPIO_SETDATAOUT       (0x0094)

/* Interrupt related stuff */
#define GPIO_IRQSTATUS1       (0x0018)
#define GPIO_IRQENABLE1       (0x001C)
#define GPIO_IRQSTATUS2       (0x0028)
#define GPIO_IRQENABLE2       (0x002C)
#define GPIO_CLEARIRQENABLE1  (0x0060)
#define GPIO_SETIRQENABLE1    (0x0064)
#define GPIO_CLEARIRQENABLE2  (0x0070)
#define GPIO_SETIRQENABLE2    (0x0074)

/* Helper macros for GPIO related stuff */
#define GPIO_PIN_POS(pin) (pin % 32) // gets the pin of a register, e.g. GPIO_149 has the address 21 of the register
#define GPIO_BASE_ADDR(pin) (pin / 32 == 0 ? GPIO1_BASE : pin / 32 == 1 ? GPIO2_BASE : pin / 32 == 2 ? GPIO3_BASE : pin / 32 == 3 ? GPIO4_BASE : pin / 32 == 4 ? GPIO5_BASE : pin / 32 == 5 ? GPIO6_BASE : 0)
#define GPIO_ADDR(_register, pin)  (GET_GPIO_BASE_ADDR(pin) + _register)// Returns the full address for the given GPIO-pin (e.g. GPIO_149)


/* GPIO Pin definitions */

/* GPIO 1 Module 0:31 */
#define GPIO_0 0
#define GPIO_1 1
#define GPIO_2 2
#define GPIO_3 3
#define GPIO_4 4
#define GPIO_5 5
#define GPIO_6 6
#define GPIO_7 7
#define GPIO_8 8
#define GPIO_9 9
#define GPIO_10 10
#define GPIO_11 11
#define GPIO_12 12
#define GPIO_13 13
#define GPIO_14 14
#define GPIO_15 15
#define GPIO_16 16
#define GPIO_17 17
#define GPIO_18 18
#define GPIO_19 19
#define GPIO_20 20
#define GPIO_21 21
#define GPIO_22 22
#define GPIO_23 23
#define GPIO_24 24
#define GPIO_25 25
#define GPIO_26 26
#define GPIO_27 27
#define GPIO_28 28
#define GPIO_29 29
#define GPIO_30 30
#define GPIO_31 31

// and so on ...

#define GPIO_149 149
#define GPIO_150 150

#endif /* OMAP3530GPIO_H_ */
