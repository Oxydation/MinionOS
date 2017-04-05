/*
 * Timer.c
 *
 *  Created on: 5 Apr 2017
 *      Author: Mathias
 */

#include "kernel/devices/omap3530/includes/Timer.h"
#include "kernel/hal/timer/Timer.h"
#include "kernel/devices/omap3530/includes/Interrupts.h"

uint32_t get_timer_address(TimerNumber timerNumber)
{
    switch (timerNumber)
    {
    case TIMER1:
        return GPTIMER1_BASE;
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
        return GPTIMER1_BASE;
    }
}

uint32_t get_irq_number(TimerNumber timerNumber){
    switch (timerNumber)
       {
       case TIMER1:
           return GPT1_IRQ;
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
           return GPT1_IRQ;
       }
}

TimerNumber get_timer_number_from_irq_source(uint32_t irq_number){
    switch (irq_number)
          {
          case GPT1_IRQ:
              return TIMER1;
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
              return TIMER1;
          }
}
