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
    TIMER2 = 0,
    TIMER3 = 1,
    TIMER4 = 2,
    TIMER5 = 3,
    TIMER6 = 4 ,
    TIMER7 = 5,
    TIMER8 = 6,
    TIMER9 = 7,
    TIMER10 = 8,
    TIMER11 = 9,
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
    uint8_t initialized;
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
static int8_t get_free_timer_index(void);
static uint32_t get_clock_rate(uint32_t interval_us);
static float get_s_from_us(uint32_t us);

#endif /* KERNEL_HAL_TIMER_TIMER_H_ */
