/*
 * Timer.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_TIMER_TIMER_H_
#define KERNEL_HAL_TIMER_TIMER_H_

#include "../../common/mmio.h"
#include "kernel/systemModules/processManagement/contextSwitch.h"

#define AMOUNT_OF_TIMERS 10
#define MAX_TIMER_LOAD_VAL 0xFFFFFFFE

#define CLK_32KHZ 32768 // clock period

typedef void (*TickCallback_t)(PCB_t *);

// TIMER1 is not being used,
// as setting the clock source is not a standard procedure
typedef enum
{
    TIMER2 = 0,
    TIMER3 = 1,
    TIMER4 = 2,
    TIMER5 = 3,
    TIMER6 = 4,
    TIMER7 = 5,
    TIMER8 = 6,
    TIMER9 = 7,
    TIMER10 = 8,
    TIMER11 = 9,
    UNAVAILABLE = -1
} TimerNumber_t;

typedef enum
{
    AUTORELOAD, ONE_SHOT
} ReloadType_t;

typedef enum
{
    NONE, COMPARE, CAPTURE, OVERFLOW
} TimerMode_t;

typedef struct Timer
{
    uint8_t initialized;
    uint32_t interval_us;
    TimerNumber_t timerNr;
    TimerMode_t timerMode;
    ReloadType_t reloadType;
    TickCallback_t callback;
} Timer_t;

/* Public functions of timer construct*/
Timer_t * timer_create(TimerMode_t mode, ReloadType_t reloadType,
                       uint32_t interval_us, TickCallback_t callback);
void timer_clearInterruptFlag(Timer_t * timer);
void timer_start(Timer_t * timer);
void timer_stop(Timer_t * timer);
void timer_resetCounter(Timer_t * timer);

/* Important exernal device dependend implementations  */
TimerNumber_t timer_getTimerNumberFromIrqSource(uint32_t irq_number);
uint32_t timer_getIrqNumber(TimerNumber_t timerNumber);
uint32_t timer_getTimerAddress(TimerNumber_t timerNumber);
void timer_setTimerLoadValue(TimerNumber_t timerNumber, uint32_t loadValue);
void timer_setTimerInterruptEnabled(TimerNumber_t timerNumber,
                                        TimerMode_t mode);
void omapTimer_clearInterruptFlag(TimerNumber_t timerNumber);
void omapTimer_start(TimerNumber_t timerNumber, ReloadType_t reloadType);
void omapTimer_stop(TimerNumber_t timerNumber);

#endif /* KERNEL_HAL_TIMER_TIMER_H_ */
