/*
 * Timer.h
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#ifndef KERNEL_HAL_TIMER_TIMER_H_
#define KERNEL_HAL_TIMER_TIMER_H_

#include "kernel/common/Common.h"

#define AMOUNT_OF_TIMERS 10
#define MAX_TIMER_LOAD_VAL 0xFFFFFFFE

#define CLK_32KHZ 32768 // clock period

typedef void (*TickCallback_t)(void);

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
} TimerNumber;

typedef enum
{
    AUTORELOAD, ONE_SHOT
} ReloadType;

typedef enum
{
    NONE, COMPARE, CAPTURE, OVERFLOW
} TimerMode;

typedef struct Timer
{
    uint8_t initialized;
    uint32_t interval_us;
    TimerNumber timerNr;
    TimerMode timerMode;
    ReloadType reloadType;
    TickCallback_t callback;
} Timer_t;

/* Public functions of timer construct*/
Timer_t * timer_create(TimerMode mode, ReloadType reloadType,
                       uint32_t interval_us, TickCallback_t callback);
void timer_clearInterruptFlag(Timer_t * timer);
void timer_start(Timer_t * timer);
void timer_stop(Timer_t * timer);

/* Important exernal device dependend implementations  */
extern TimerNumber timer_getTimerNumberFromIrqSource(uint32_t irq_number);
extern uint32_t timer_getIrqNumber(TimerNumber timerNumber);
extern uint32_t timer_getTimerAddress(TimerNumber timerNumber);
extern void timer_setTimerLoadValue(TimerNumber timerNumber, uint32_t loadValue);
extern void timer_setTimerInterruptEnabled(TimerNumber timerNumber,
                                        TimerMode mode);
extern void omapTimer_clearInterruptFlag(TimerNumber timerNumber);
extern void omapTimer_start(TimerNumber timerNumber, ReloadType reloadType);
extern void omapTimer_stop(TimerNumber timerNumber);

/* Private functions */
static void init_timer(Timer_t * timer);
static int8_t getFreeTimerIndex(void);
static void isr_handler(uint32_t source);
static uint32_t getClockRateFromInterval(uint32_t interval_us);
static float getSecondsFromMicroseconds(uint32_t microseconds);
static uint32_t calcLoadValue(uint32_t clockRate, uint32_t interval_us);


#endif /* KERNEL_HAL_TIMER_TIMER_H_ */
