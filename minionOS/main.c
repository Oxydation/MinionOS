#include <drivers/dmx/mhx25/dmxMhx25.h>
#include <drivers/dmx/mhx25/dmxMhx25.h>
#include <stdio.h>
#include <inttypes.h>

#include <kernel/common/mmio.h>
#include <kernel/devices/omap3530/includes/beagleBoardC4.h>
#include <kernel/devices/omap3530/includes/modeSwitch.h>
#include <kernel/hal/gpio/gpio.h>
#include <kernel/hal/interrupts/interrupts.h>
#include <kernel/hal/timer/systemTimer.h>
#include "global/types.h"
#include "kernel/systemModules/scheduler/scheduler.h"
#include "kernel/systemModules/processManagement/processManager.h"
#include "kernel/hal/dmx/dmx.h"
#include "drivers/dmx/tmh7/dmxTmh7.h"
#include "drivers/dmx/mhx25/dmxMhx25.h"
#include <string.h>
#include "global/delay/delay.h"
#include "applications/systemCallApi.h"
#include "kernel/systemModules/filesystem/vfs.h"
#include "applications/shell/shell.h"
#include "kernel/hal/mmc_sd/sdCard.h"
#include <stdlib.h>
#include "kernel/hal/uart/uart.h"

void process1(void);
void process2(void);

int main(void)
{
    _disable_interrupts();
    interrupts_initIrq();

    // Set output direction
    gpio_pinMode(GPIO_USR0_LED, OUTPUT);
    gpio_pinMode(GPIO_USR1_LED, OUTPUT);

    UartConfig_t uartConfig = { .baudMultiple = x16, .baudRate = 115200,
                                .stopMode = STOP_1, .parityMode = NO_PARITY,
                                .wordLength = LENGTH_8 };

    // Initialization
    dmx_init();
    uart_initModule(UART3, uartConfig);
    sdCard_initialize_Ch1();
    vfs_init();
    systemTimer_init(1000);
    scheduler_init();

    processManager_loadProcess(&process1 + 0x4, 0x80607500);
    //processManager_loadProcess(&process2 + 0x4, 0x8060FF00);

    _enable_interrupts();
    _enable_IRQ();

    systemTimer_start();
    scheduler_start();

    modeSwitch_switchToUserMode();

    while (1)
    {

    }
}

typedef struct
{
    uint8_t * question;
    uint8_t * answer1;
    uint8_t * answer2;
    uint8_t * answer3;
    uint8_t correctAnswerNumber;
} QuizQuestion_t;

#pragma CODE_SECTION(process1,".process1") // DDR0_PROC1: o = 0x80600000
void process1(void)
{
    DmxDataTMH7_t data = { .pan = 0, .tilt = 100, .green = 10, .blue = 128,
                           .red = 10, .dimmingIntensity = 50, .colorMacro = 0,
                           .ledCircuit = 255, .speed = 50,

    };
    DmxDataMhX25_t dataMhx25 = { .pan = 255, .tilt = 100, .speed = 190,
                                 .dimmer = 100, .colour = YellowGreen, .gobo =
                                         Gobo5,
                                 .goboRotation = 150, .shutter = 5 };

    int quizHandle = sysCalls_openFile("QUIZ.TXT");

    if (quizHandle < 0)
    {
        // not able to open file
        return;
    }
    uint8_t quizFile[1400] = { 0 };
    sysCalls_readFile(quizHandle, &quizFile, 1400);
    uint16_t amountChars = 1390;
    QuizQuestion_t questions[20];

    int currPos = 0;
    int currentQuizQuestion = 0;
    while (amountChars > currPos)
    {
        int column = 0;

        // Rows
        while (amountChars > currPos && quizFile[currPos] != 10 && column < 5)
        {
            uint8_t * currentColumnContent;
            currentColumnContent = (uint8_t*) malloc(60 * sizeof(uint8_t));

            // seems to have no more memory available :SS
            if(currentColumnContent == 0x00){
                uart_transmit(UART3, "no memory available\n", 20);
                return;
            }
            int pos = 0;

            while (amountChars > currPos && quizFile[currPos] != '\t' && quizFile[currPos] != 10)
            {
                currentColumnContent[pos++] = quizFile[currPos++];
            }
            currPos++;

            uart_transmit(UART3, currentColumnContent, pos);

            switch (column)
            {
            case 0:
                questions[currentQuizQuestion].question = currentColumnContent;
                break;
            case 1:
                questions[currentQuizQuestion].answer1 = currentColumnContent;
                break;
            case 2:
                questions[currentQuizQuestion].answer2 = currentColumnContent;
                break;
            case 3:
                questions[currentQuizQuestion].answer3 = currentColumnContent;
                break;
            case 4:
                questions[currentQuizQuestion].correctAnswerNumber =
                        *currentColumnContent;

                currentQuizQuestion++;
                break;
            }

            column++;
        }
    }

    while (1)
    {
        delay(100000);
        //dataMhx25.pan += 3;
        data.pan += 3;
//
        uint16_t packetSize = 26;
        uint8_t packet[26];
        memset(packet, 0, packetSize * sizeof(uint8_t));

        packet[0] = 0x00;

        dmx_createMhX25Packet(1, &dataMhx25, &packet);
        dmx_createTmh7Packet(13, &data, &packet);

        //dmx_send(&packet, packetSize);
        sysCalls_ctrlDmx(&packet, packetSize);
    }
}

#pragma CODE_SECTION(process2,".process2") //  DDR0_PROC2: o = 0x80608000
void process2(void)
{
    volatile unsigned long i = 0;
    uint32_t* out = (uint32_t*) (GPIO_BASE_ADDR(GPIO_USR1_LED) + GPIO_DATAOUT);

    while (1)
    {
        bitClear(*out, GPIO_PIN_POS(GPIO_USR1_LED));
    }
}
