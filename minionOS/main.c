#include <stdio.h>
#include <inttypes.h>
#include <kernel/hal/interrupts/interrupts.h>
#include "applications/syscallApi.h"
#include "kernel/common/Common.h"
#include "kernel/devices/omap3530/includes/Clock.h"
#include "kernel/devices/omap3530/includes/GPIO.h"
#include "kernel/devices/omap3530/includes/Interrupts.h"
#include "kernel/devices/omap3530/includes/Timer.h"
#include "kernel/systemModules/syscalls/syscallNumbers.h"
#include "kernel/hal/gpio/GPIO.h"
#include "kernel/devices/omap3530/includes/BeagleBoardC4.h"

uint8_t on = TRUE;

void handler_test(void) {
    if (on) {
        digitalWrite(GPIO_USR1_LED, HIGH);
        digitalWrite(GPIO_USR2_LED, LOW);
        on = FALSE;
    }
    else {
        digitalWrite(GPIO_USR1_LED, LOW);
        digitalWrite(GPIO_USR2_LED, HIGH);
        on = TRUE;
    }

    set_32(GPTIMER2_BASE + GPTIMER_TISR, (1 << 1)); // CLear interrupt flag
}

int main(void) {

    //  _disable_interrupts();

    //   init_irq();
    /*
     // Clear timer load value and set to 0xFFFF0000
     // Check p.2614 for example values for the load value
     set_32(GPTIMER2_BASE + GPTIMER_TLDR, 0xFFFF829B);
     set_32(GPTIMER2_BASE + GPTIMER_TCRR, 0xFFFF829B);

     // Set clock source to 32kHz
     clear_32(PER_CM + CM_CLKSEL_PER, (1 << 0));

     // Enable Overflow interrupt
     set_32(GPTIMER2_BASE + GPTIMER_TIER,
     TIER_TCAR_IT_DISABLE | TIER_OVF_IT_ENABLE | TIER_MAT_IT_DISABLE);

     set_32(GPTIMER2_BASE + GPTIMER_TISR, (1 << 1)); // CLear interrupt flag

     // Register handler and enable global interrupts
     register_interrupt_handler(&handler_test, GPT2_IRQ);*/
    _enable_interrupts();

    //   _enable_IRQ();

    // Turn on GPTIMER2, it will reload at overflow
    /*  set_32(GPTIMER2_BASE + GPTIMER_TCLR, TCLR_AR_AUTORELOAD | TCLR_ST_ON);

     // Set output direction
     pinMode(GPIO_USR1_LED, OUTPUT);
     pinMode(GPIO_USR2_LED, OUTPUT);*/

    //_call_swi(0);
    while (1) {
        syscalls_controlLed(TRUE, 1);
        syscalls_controlLed(FALSE, 2);
        volatile unsigned int i = 1000000;                          // SW Delay
        do
            i--;
        while (i != 0);
        syscalls_controlLed(FALSE, 1);
        syscalls_controlLed(TRUE, 2);
        i = 1000000;                          // SW Delay
        do
            i--;
        while (i != 0);
    }
    return 0;
}
