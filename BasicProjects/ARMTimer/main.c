#include <stdio.h>
#include <inttypes.h>
#include "includes/BeagleBoardC4.h"
#include "includes/Common.h"
#include "includes/Omap3530GPIO.h"
#include "includes/Omap3530Timer.h"
#include "includes/Interrupts.h"
#include "includes/Omap3530Clock.h"

uint8_t on = TRUE;

void handler_test(void)
{
    if (on)
    {
        digitalWrite(GPIO_USR1_LED, HIGH);
        digitalWrite(GPIO_USR2_LED, LOW);
        on = FALSE;
    }
    else
    {
        digitalWrite(GPIO_USR1_LED, LOW);
        digitalWrite(GPIO_USR2_LED, HIGH);
        on = TRUE;
    }

    set_32(GPTIMER2_BASE + GPTIMER_TISR, (1 << 1)); // CLear interrupt flag
}

int main(void)
{
    _disable_interrupts();

    init_irq();

    set_32(GPTIMER2_BASE + GPTIMER_TISR, (1 << 1)); // CLear interrupt flag

    register_interrupt_handler(&handler_test, GPT2_IRQ);

    // Clear timer load value and set to 0xFFFF0000
    set_32(GPTIMER2_BASE + GPTIMER_TLDR, 0xFFFF0000);

    // Set clock source to 32kHz
    clear_32(PER_CM + CM_CLKSEL_PER, (0<<0));

    // Enable Overflow interrupt
    set_32(GPTIMER2_BASE + GPTIMER_TIER,
    TIER_TCAR_IT_DISABLE | TIER_OVF_IT_ENABLE | TIER_MAT_IT_DISABLE);


    _enable_interrupts();
     _enable_IRQ();

    // Turn on GPTIMER2, it will reload at overflow
    set_32(GPTIMER2_BASE + GPTIMER_TCLR, TCLR_AR_AUTORELOAD | TCLR_ST_ON);

    // Set output direction
    pinMode(GPIO_USR1_LED, OUTPUT);
    pinMode(GPIO_USR2_LED, OUTPUT);

    //_call_swi(0);
    while (1)
    {

    }
    return 0;
}
