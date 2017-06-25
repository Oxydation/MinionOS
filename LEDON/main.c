/*
 * main.c
 */

#include "systemCallApi.h"

int main(void)
{
    while (1)
    {
        sysCalls_enableLed(1, LED_0);
    }
}
