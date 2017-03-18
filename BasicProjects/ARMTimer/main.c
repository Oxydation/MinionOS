#include <includes/Common.h>
#include <includes/Omap3530GPIO.h>
#include <includes/Omap3530Timer.h>
#include <stdio.h>
#include <inttypes.h>
#include "includes/BeagleBoardC4.h"

int main(void)
{
    volatile long i = 0;
    printf("Hello World!\n");

    // Set output direction
    pinMode(GPIO_USR1_LED, OUTPUT);
    pinMode(GPIO_USR2_LED, OUTPUT);

    while (1)
    {
        // Turn LED1 on and LED 2 off
        digitalWrite(GPIO_USR1_LED, HIGH);
        digitalWrite(GPIO_USR2_LED, LOW);
        for (i = 0; i < 2000000L; i++)
        {

        }

        // Turn LED 1 off and LED 2 on
        digitalWrite(GPIO_USR1_LED, LOW);
        digitalWrite(GPIO_USR2_LED, HIGH);
        for (i = 0; i < 2000000L; i++)
        {

        }
    }
    return 0;
}
