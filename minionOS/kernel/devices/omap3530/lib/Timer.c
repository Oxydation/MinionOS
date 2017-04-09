/*
 * Timer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include "kernel/devices/omap3530/includes/Timer.h"
#include "kernel/hal/timer/Timer.h"
#include "kernel/devices/omap3530/includes/Interrupts.h"

uint32_t timer_getTimerAddress(TimerNumber timerNumber)
{
    switch (timerNumber)
    {
//    case TIMER1:
//        return GPTIMER1_BASE;
    case TIMER2:
        return GPTIMER2_BASE;
    case TIMER3:
        return GPTIMER3_BASE;
    case TIMER4:
        return GPTIMER4_BASE;
    case TIMER5:
        return GPTIMER5_BASE;
    case TIMER6:
        return GPTIMER6_BASE;
    case TIMER7:
        return GPTIMER7_BASE;
    case TIMER8:
        return GPTIMER8_BASE;
    case TIMER9:
        return GPTIMER9_BASE;
    case TIMER10:
        return GPTIMER10_BASE;
    case TIMER11:
        return GPTIMER11_BASE;
    default:
        return GPTIMER2_BASE;
    }
}

uint32_t timer_getIrqNumber(TimerNumber timerNumber)
{
    switch (timerNumber)
    {
//       case TIMER1:
//           return GPT1_IRQ;
    case TIMER2:
        return GPT2_IRQ;
    case TIMER3:
        return GPT3_IRQ;
    case TIMER4:
        return GPT4_IRQ;
    case TIMER5:
        return GPT5_IRQ;
    case TIMER6:
        return GPT6_IRQ;
    case TIMER7:
        return GPT7_IRQ;
    case TIMER8:
        return GPT8_IRQ;
    case TIMER9:
        return GPT9_IRQ;
    case TIMER10:
        return GPT10_IRQ;
    case TIMER11:
        return GPT11_IRQ;
    default:
        return GPT2_IRQ;
    }
}

TimerNumber timer_getTimerNumberFromIrqSource(uint32_t irq_number)
{
    switch (irq_number)
    {
//          case GPT1_IRQ:
//              return TIMER1;
    case GPT2_IRQ:
        return TIMER2;
    case GPT3_IRQ:
        return TIMER3;
    case GPT4_IRQ:
        return TIMER4;
    case GPT5_IRQ:
        return TIMER5;
    case GPT6_IRQ:
        return TIMER6;
    case GPT7_IRQ:
        return TIMER7;
    case GPT8_IRQ:
        return TIMER8;
    case GPT9_IRQ:
        return TIMER9;
    case GPT10_IRQ:
        return TIMER10;
    case GPT11_IRQ:
        return TIMER11;
    default:
        return UNAVAILABLE;
    }
}

void timer_setTimerLoadValue(TimerNumber timerNumber, uint32_t loadValue)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timerNumber);

    // Clear timer load value and set to a given load value
    // Check p.2614 for example values for the load value
    set32(timerBaseAddress + GPTIMER_TLDR, loadValue);
    set32(timerBaseAddress + GPTIMER_TCRR, loadValue);
}

void timer_setTimerInterruptEnabled(TimerNumber timerNumber, TimerMode mode)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timerNumber);
    uint32_t tier = 0;
    switch (mode)
    {
    case OVERFLOW:
        tier = TIER_OVF_IT_ENABLE;
        break;
    case COMPARE:
        tier = TIER_MAT_IT_ENABLE;
        break;
    case CAPTURE:
        tier = TIER_TCAR_IT_ENABLE;
        break;
    default:
        tier = 0;
        break;
    }

    // Set interrupt enabled for either overflow, compare or capture
    set32(timerBaseAddress + GPTIMER_TIER, tier);

}

void omapTimer_clearInterruptFlag(TimerNumber timerNumber)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timerNumber);
    set32(timerBaseAddress + GPTIMER_TISR, (1 << 1)); // Clear interrupt flag
}

void omapTimer_start(TimerNumber timerNumber, ReloadType reloadType)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timerNumber);

    uint32_t reloadMode;

    if (reloadType == AUTORELOAD)
    {
        reloadMode = TCLR_AR_AUTORELOAD;
    }
    else
    {
        reloadMode = TCLR_AR_ONESHOT;
    }

    set32(timerBaseAddress + GPTIMER_TCLR, reloadMode | TCLR_ST_ON);
}

void omapTimer_stop(TimerNumber timerNumber)
{
    uint32_t timerBaseAddress = timer_getTimerAddress(timerNumber);
    clear32(timerBaseAddress + GPTIMER_TCLR, TCLR_ST_ON);
}
