/*
 * main.c
 */

#include "systemCallApi.h"

int main(void)
{
    while (1)
    {
        sysCalls_enableLed(0, LED_0);
    }
}
