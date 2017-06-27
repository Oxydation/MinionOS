/*
 * game123.c
 *
 *  Created on: 21 Jun 2017
 *      Author: Mathias
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "systemCallApi.h"
#include "minionIO.h"
#include "drivers/dmx/tmh7/dmxTmh7.h"
#include "drivers/dmx/mhx25/dmxMhx25.h"
#include "delay/delay.h"


uint16_t packetSize = 40;
uint8_t packet[40];
uint8_t alreadyUsedQuestions[20];

DmxDataTMH7_t g_tmhData_1[3] = { { .pan = 0, .tilt = 225, .green = 0, .blue = 0,
                                   .red = 255, .dimmingIntensity = 255,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, },
                                 { .pan = 0, .tilt = 180, .green = 255, .blue =
                                           0,
                                   .red = 255, .dimmingIntensity = 255,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, },
                                 { .pan = 0, .tilt = 145, .green = 0, .blue =
                                           255,
                                   .red = 0, .dimmingIntensity = 255,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, } };

DmxDataTMH7_t g_tmhData_2[3] = { { .pan = 0, .tilt = 115, .green = 0, .blue = 0,
                                   .red = 255, .dimmingIntensity = 255,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, },
                                 { .pan = 0, .tilt = 160, .green = 255, .blue =
                                           0,
                                   .red = 255, .dimmingIntensity = 200,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, },
                                 { .pan = 0, .tilt = 200, .green = 0, .blue =
                                           255,
                                   .red = 0, .dimmingIntensity = 255,
                                   .colorMacro = 0, .ledCircuit = 255, .speed =
                                           50, } };

DmxDataMhX25_t g_mhxData = { .pan = 0, .tilt = 128, .speed = 10, .dimmer = 255,
                             .colour = RainbowClockwise, .gobo = Gobo2Shake,
                             .goboRotation = 240, .shutter = 190 };

DmxDataMhX25_t g_mhxData123[3] = { { .pan = 40, .tilt = 97, .speed = 10,
                                     .dimmer = 255, .colour = Red,
                                     .gobo = Gobo2, .goboRotation = 240,
                                     .shutter = 190 },
                                   { .pan = 68, .tilt = 102, .speed = 10,
                                     .gobo = Gobo2, .goboRotation = 240,
                                     .dimmer = 255, .colour = Yellow, .shutter =
                                             190 },
                                   { .pan = 98, .tilt = 97, .speed = 10, .gobo =
                                             Gobo2,
                                     .goboRotation = 240, .dimmer = 255,
                                     .colour = LightBlue, .shutter = 190 } };

DmxDataMhX25_t g_mhxDataReset = { .pan = 0, .tilt = 128, .speed = 190, .dimmer =
                                          0,
                                  .colour = YellowGreen, .shutter = 5 };

DmxDataTMH7_t g_tmhDataReset = { .pan = 0, .tilt = 128, .green = 0, .blue = 0,
                                 .red = 0, .dimmingIntensity = 0, .colorMacro =
                                         0,
                                 .ledCircuit = 255, .speed = 100, };

typedef struct
{
    uint8_t * question;
    uint8_t * answer1;
    uint8_t * answer2;
    uint8_t * answer3;
    uint8_t correctAnswerNumber;
} QuizQuestion_t;

static QuizQuestion_t g_questions[30];

static uint16_t readQuizFile(uint8_t * target)
{
    int quizHandle = sysCalls_openFile("/QUIZ.TXT");
    uint16_t size = 1400;

    if (quizHandle < 0)
    {
        minionIO_writeln("Not able to open quiz file.");
        // not able to open file
        return 0;
    }

    sysCalls_readFile(quizHandle, target, size);
    return size;
}

static uint8_t parseQuizData(const uint8_t * data, uint16_t amountChars,
                             QuizQuestion_t * questions)
{
    int currPos = 0;
    int currentQuizQuestion = 0;
    while (amountChars > currPos)
    {
        int column = 0;

        // Rows
        while (amountChars > currPos && data[currPos] != 10 && column < 5)
        {
            uint8_t * currentColumnContent;
            currentColumnContent = (uint8_t*) malloc(64 * sizeof(uint8_t));

            // seems to have no more memory available :S
            if (currentColumnContent == 0x00)
            {
                // FREE MEMORY
                minionIO_writeln("No memory available");
                return 1;
            }

            int pos = 0;

            while (amountChars > currPos && data[currPos] != '\t'
                    && data[currPos] != 10)
            {
                currentColumnContent[pos++] = data[currPos++];
            }
            currentColumnContent[pos++] = '\0';

            currPos++;

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
                        (*currentColumnContent) - 48; // get int value of char

                currentQuizQuestion++;
                break;
            }

            column++;
        }
    }

    return currentQuizQuestion;
}

static void highlightAnswer(uint8_t answerNumber)
{
    //dmx_createMhX25Packet(1, &g_mhxDataReset, &packet);
    dmx_createMhX25Packet(1, &g_mhxData123[answerNumber - 1], &packet);
    dmx_createTmh7Packet(13, &g_tmhData_1[answerNumber - 1], &packet);
    dmx_createTmh7Packet(25, &g_tmhData_2[answerNumber - 1], &packet);
    sysCalls_ctrlDmx(&packet, packetSize);
}

static void resetMovingHeads(void)
{
    dmx_createMhX25Packet(1, &g_mhxDataReset, &packet);
    dmx_createTmh7Packet(13, &g_tmhDataReset, &packet);
    dmx_createTmh7Packet(25, &g_tmhDataReset, &packet);
    sysCalls_ctrlDmx(&packet, packetSize);
}

static void discoLight(uint8_t rounds)
{
    int counter = 0;
    while (counter < rounds)
    {
        g_mhxData.pan = rand() % 255;

        if (g_mhxData.pan > 128 && g_mhxData.pan < 190)
        {
            g_mhxData.tilt = 90 + rand() % 80;
        }
        else
        {
            g_mhxData.tilt = 120 + rand() % 60;
        }
        dmx_createMhX25Packet(1, &g_mhxData, &packet);
        dmx_createTmh7Packet(13, &g_tmhData_1[rand() % 3], &packet);
        dmx_createTmh7Packet(25, &g_tmhData_2[rand() % 3], &packet);
        sysCalls_ctrlDmx(&packet, packetSize);
        delay(600000);
        counter++;
    }
}

static void playGame(QuizQuestion_t * questions, uint8_t amountOfQuestions)
{
    resetMovingHeads();

    uint8_t currentQuestionPos = 0;
    QuizQuestion_t * currentQuestion;

    memset(alreadyUsedQuestions, FALSE, amountOfQuestions * sizeof(uint8_t));
    memset(packet, 0, packetSize * sizeof(uint8_t));
    packet[0] = 0x00;

    for (currentQuestionPos = 0; currentQuestionPos < amountOfQuestions;
            currentQuestionPos++)
    {
        uint8_t randomQuestionPos = rand() % amountOfQuestions;
        while (alreadyUsedQuestions[randomQuestionPos])
        {
            randomQuestionPos = rand() % amountOfQuestions;
        }
        alreadyUsedQuestions[randomQuestionPos] = TRUE;
        resetMovingHeads();
        currentQuestion = &questions[randomQuestionPos];

        // Show question
        minionIO_writeln(currentQuestion->question);

        discoLight(20);
        resetMovingHeads();
        delay(3000000);

        minionIO_writeln(currentQuestion->answer1);
        highlightAnswer(1);
        delay(5000000);

        minionIO_writeln(currentQuestion->answer2);
        highlightAnswer(2);
        delay(5000000);

        minionIO_writeln(currentQuestion->answer3);
        highlightAnswer(3);

        delay(5000000);
        discoLight(20);

        // minionIO_skipLn();
        minionIO_writeln("1..2..oder 3...letzte Chance vorbei.");
        discoLight(9);
        resetMovingHeads();
        minionIO_writeln(
                "Ob du wirklich richtig stehst, siehst du wenn das Licht angeht!");
        delay(9000000);

//        char buf[15];
//        sprintf(buf, "%d", currentQuestion->correctAnswerNumber);
//        minionIO_writeln(&buf);

        uint8_t * correntAnswer;

        switch (currentQuestion->correctAnswerNumber)
        {
            case 1:
                correntAnswer = currentQuestion->answer1;
                break;
            case 2:
                correntAnswer = currentQuestion->answer2;
                break;
            case 3:
                correntAnswer = currentQuestion->answer3;
                break;
        }
        minionIO_writeln(correntAnswer);
        delay(1000);

        highlightAnswer(currentQuestion->correctAnswerNumber);
        delay(1000000);

        // wait for enter for next question
        minionIO_skipLn();
        resetMovingHeads();
    }
    minionIO_writeln("Das Spiel ist nun beendet.");
    game123_stopGame();
}

static void prepareDmx(void)
{
    uint8_t i = 0;
    while (i++ < 200)
    {
        delay(2000);
        sysCalls_ctrlDmx(&packet, packetSize);
    }
}

int game123_stopGame()
{
    //TODO Abort running ame
    resetMovingHeads();
    return 0;
}

int game123_main(void)
{
    srand(time(NULL));
    minionIO_writeln("Das Spiel wird vorbereitet...Bitte warten...");
    prepareDmx();

    uint8_t quizFile[1400] = { 0 };
    uint16_t amountChars = readQuizFile(&quizFile);

    if (amountChars == 0)
    {
        return 1;
    }

    uint8_t amountOfQuestions = parseQuizData(&quizFile, amountChars,
                                              &g_questions);

    playGame(&g_questions, amountOfQuestions);

    return 0;
}
