/*
 * main.c
 */

#include "systemCallApi.h"
#include "minionIO.h"

int main(void)
{
    sysCalls_initSemaphore("uart", 1);
    while (1)
    {
        int i;
        sysCalls_pSemaphore("uart");
        for (i = 0; i < 2000; i++) {
            minionIO_write("on");
        }
        sysCalls_vSemaphore("uart");
    }
}
