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

typedef void (*TickCallback_t)(void);

typedef enum
{
    TIMER1 = 0,
    TIMER2 = 1,
    TIMER3 = 2,
    TIMER4 = 3,
    TIMER5 = 4,
    TIMER6 = 5 ,
    TIMER7 = 6,
    TIMER8 = 7,
    TIMER9 = 8,
    TIMER10 = 9,
    TIMER11 = 10,
    UNAVAILABLE = -1
} TimerNumber;

typedef enum
{
    AUTORELOAD,
    ONE_SHOT
} ReloadType;

typedef enum{
    NONE,
    COMPARE,
    CAPTURE,
    OVERFLOW
} TimerMode;

typedef struct Timer{
    uint32_t interval_us;
    TimerNumber timerNr;
    TimerMode timerMode;
    ReloadType reloadType;
    TickCallback_t callback;
} Timer_t;

/* Public functions to setup timer */
Timer_t * create_timer(TimerMode mode, ReloadType reloadType, uint32_t interval_us, TickCallback_t callback);
void clear_interrupt_flag(Timer_t  * timer);
void start_timer(Timer_t * timer);
void stop_timer(Timer_t * timer);


extern TimerNumber get_timer_number_from_irq_source(uint32_t irq_number);
extern uint32_t get_irq_number(TimerNumber timerNumber);
extern uint32_t get_timer_address(TimerNumber timerNumber);
static uint32_t calc_load_value(uint32_t clockRate, uint32_t interval_us);
static void init_timer(Timer_t * timer);
static TimerNumber get_free_timer();
static uint32_t get_clock_rate(uint32_t interval_us);
static float get_s_from_us(uint32_t us);

#endif /* KERNEL_HAL_TIMER_TIMER_H_ */
