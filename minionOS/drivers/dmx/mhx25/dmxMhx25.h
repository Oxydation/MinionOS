/*
 * dmxMx25.h
 *
 *  Created on: 7 Jun 2017
 *      Author: Mathias
 */

#ifndef DRIVERS_DMX_MHX25_DMXMHX25_H_
#define DRIVERS_DMX_MHX25_DMXMHX25_H_

#include "global/types.h"
#include "kernel/hal/dmx/dmx.h"

#define DMX_CHANNELS_MHX25 12

typedef enum {
    White = 0,
    Yellow = 5,
    Pink = 10,
    Green = 15,
    Peachblow = 20,
    LightBlue = 25,
    YellowGreen = 30,
    Red = 35,
    DarkBlue = 40,
    TransWhiteToYellow = 50,
    TransYellowToPink = 60,
    TransPinkToGreen = 70,
    TransGreenToPeachblow = 80,
    TransPeachblowToLightBlue = 90,
    TransLightBlueToYellowGreen = 95,
    TransYellowGreenToRed = 105,
    TransRedToDarkBlue = 115,
    TransDarkBlueToWhite = 125,
    RainbowClockwise = 128, // up to 191, increasing speed
    RainbowCounterClockwise = 192 // up to 255, increasing speed
}ColourMhx25_t;

typedef enum{
    Open,
    Gobo2 = 12,
    Gobo3 = 19,
    Gobo4 = 28,
    Gobo5 = 35,
    Gobo6 = 44,
    Gobo7 = 51,
    Gobo8 = 58,
    Gobo8Shake = 64,
    Gobo7Shake = 72,
    Gobo6Shake = 80,
    Gobo5Shake = 88,
    Gobo4Shake = 96,
    Gobo3Shake = 104,
    Gobo2Shake = 112,
}GoboEffect_t;

typedef struct
{
    uint8_t startCode;

    uint8_t pan;    // 0 up to 255
    uint8_t tilt;   // 0 up to 255

    uint8_t panFine;
    uint8_t tiltFine;

    uint8_t speed;

    uint8_t colour;

    uint8_t shutter;

    uint8_t dimmer;

    uint8_t gobo;
    uint8_t goboRotation;

    uint8_t specialFunctions;
    uint8_t programmes;
} DmxDataMhX25_t;

void dmx_createMhX25Packet(uint16_t startChannel, const DmxDataMhX25_t * data, uint8_t * result);


#endif /* DRIVERS_DMX_MHX25_DMXMHX25_H_ */
