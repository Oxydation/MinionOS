#include <stdio.h>
#include "OMAP33530GPIO.h"

int main(void)
{
    volatile long i = 0;
    printf("Hello World!\n");

    // Set output direction
    unsigned long* gpio_dir_ctr = GPIO5_BASE + GPIO_OE;
    *gpio_dir_ctr = (0 << 21) | (0 << 22);

    // Set values of ports
    unsigned long* gpio_output = GPIO5_BASE + GPIO_DATAOUT;

    while (1)
    {
        // Turn LED 149 on and LED 150 off
        *gpio_output |= GPIO_150;
        *gpio_output &= ~GPIO_149;
        for (i = 0; i < 2000000L; i++)
        {

        }

        // Turn LED 149 off and LED 150 on
        *gpio_output &= ~GPIO_150;
        *gpio_output |= GPIO_149;
        for (i = 0; i < 2000000L; i++)
        {

        }
    }
    return 0;
}
