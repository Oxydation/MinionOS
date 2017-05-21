/*
 * Omap3530SDCard.c
 *
 *  Created on: 08.04.2017
 *      Author: benimurza
 */
#include "sdCard.h"
#include "kernel/common/mmio.h"
#include "kernel/hal/timer/timer.h"
#include "global/types.h"
#include "kernel/devices/omap3530/includes/beagleBoardC4.h"
#include "kernel/hal/gpio/gpio.h"
#include <stdio.h>

#define SD_SECTOR_SIZE 512

// Card address, after initialization. Initial value = 0
static uint32_t gCardAddress = 0;

/*
 * Enable functional and internal clocks for MMC module 1.
 */
static void enableMMC1Clocks(void){
    // FCLKEN
    or32(CM_FCLKEN1_CORE, (1<<CM_FCLKEN1_CORE_EN_MMC1));

    // ICLK
    or32(CM_ICLKEN1_CORE, (1<<CM_ICLKEN1_CORE_EN_MMC1));
}

/*
 * Performs a reset on channel 1. This needs to be done on initialization.
 * Function returns when software reset is completed.
 */
static void softwareReset_Ch1(void){
    or32(MMCHS1_SYSCONFIG, (1<<MMCHS_SYSCONFIG_SOFTRESET));
    while( ( get32(MMCHS1_SYSSTATUS) & (1<<MMCHS_SYSSTATUS_RESETDONE)) != (1<<MMCHS_SYSSTATUS_RESETDONE) ){
        // TODO: maybe a timeout here?
    }
}

void softwareResetCMDLine(void){
    // Perform a software reset for the CMD Line
    or32(MMCHS1_SYSCTL, (1<<MMCHS_SYSCTL_SOFTWARE_RESET_CMD_LINE));

    // Wait until previously set bit returns to 0
    while((get32(MMCHS1_SYSCTL) & (1<<MMCHS_SYSCTL_SOFTWARE_RESET_CMD_LINE)) == (1<<MMCHS_SYSCTL_SOFTWARE_RESET_CMD_LINE)){

    }
}

/*
 * Perform default capabilities initialization for MMC module 1.
 */
static void initializeDefaultCapabilities_Ch1(void){
    // Enable 1.8 V and 3.0 V on Ch1
    or32(MMCHS1_CAPA, 1<<MMCHS_CAPA_VOLTAGE_SUPPORT_3_0);
}

/*
 * Default wake up configuration for MMC module 1.
 * Enables wake-up on card interrupt.
 */
static void initializeDefaultWakeUpConfiguration_Ch1(void){
    // ENAWAKEUP
    or32(MMCHS1_SYSCONFIG, (1<<MMCHS_SYSCONFIG_ENAWAKEUP));

    // Wake-up on card interrupt
    or32(MMCHS1_HCTL, (1<<MMCHS_HCTL_WAKEUP_EVENT_EN_ON_SD_CARD_INTERRUPT));
}

/*
 * Set the bus configuration to 1.8V, 1 bit data width and then power on
 */
static void controllerBusConfiguration_Ch1(void){
    // Set open drain for broadcast (only for MMC)
    or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

    // Configure 3.0V for controller bus (SDVS)
    or32(MMCHS1_HCTL, (0x05<<MMCHS_HCTL_SD_BUS_VOLTAGE_SELECT));

    // Data width 1 bit (1 for 4) (DTW)
    //or32(MMCHS1_HCTL, (1<<MMCHS_HCTL_DATA_TRANSFER_WIDTH));
    // Explicitly set it to one line
    and32(MMCHS1_HCTL, ~(1<<MMCHS_HCTL_DATA_TRANSFER_WIDTH));

    // SD Bus power ON (0 for Off) (SDBP)
    or32(MMCHS1_HCTL, (1<<MMCHS_HCTL_SD_BUS_POWER));

    if((get32(MMCHS1_HCTL) & (1<<MMCHS_HCTL_SD_BUS_POWER)) != (1<<MMCHS_HCTL_SD_BUS_POWER)){
        // ERROR: Card does not support voltage. Maybe change it to other value
        return;
    }


    // Enable loopback for mmc_clk
    or32(CONTROL_PADCONF_MMC1_CLK, 0x100);

    // Set clock divider, to make sure at least 1 ms passes in 80 clock cycles.
    // or32(MMCHS1_SYSCTL, (0xA<<12)); -> CLK of 150 kHz
    or32(MMCHS1_SYSCTL, (6<<6)); // 0x3FF = MAX, 240 = 400kHz?

    // Enable internal clock
    or32(MMCHS1_SYSCTL, (1<<MMCHS_SYSCTL_INTERNAL_CLOCK_ENABLE));

    // TODO: this was not in flow. Enable card clock
    or32(MMCHS1_SYSCTL, (1<<MMCHS_SYSCTL_CARD_CLOCK_ENABLE));

    // Make sure internal clock is stable
    while((get32(MMCHS1_SYSCTL) & ((1<<MMCHS_SYSCTL_INTERNAL_CLOCK_STABLE)))!=(1<<MMCHS_SYSCTL_INTERNAL_CLOCK_STABLE)){
        // Wait for internal clock to be stable
    }

    // TODO: maybe set idle behavior?
}

// stupid but necessary delay
void delayAfterCommand(void){
    volatile int i = 0;
    for(i = 0; i < 20000UL; i++);
}


int32_t detectAndInitializeSdCard(void){
    // Send initialization stream
    or32(MMCHS1_CON, (1<<MMCHS_CON_INIT));

    // Write 0 in cmd register
    set32(MMCHS1_CMD, 0x00000000);

    // wait 1 ms. This is impossible, because the clock divider cannot possibly be high enough. So just wait for a bit...
    delayAfterCommand();

    // Clear flag
    or32(MMCHS1_STAT, (1<<MMCHS_STAT_COMMAND_COMPLETE));

    // End initialization stream
    and32(MMCHS1_CON, ~(1<<MMCHS_CON_INIT));

    // Clear MMCHS1_STAT register
    set32(MMCHS1_STAT, 0xFFFFFFFF);

    // NOT PART OF FLOW
    softwareResetCMDLine();

    // Send CMD 0 command (reset all cards to idle state)
    sdCard_sendCommand(CMD0);
    delayAfterCommand();
    if((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_COMPLETE)) == (1<<MMCHS_STAT_COMMAND_COMPLETE)){
        // TODO: not part of flow. Clear flag
        or32(MMCHS1_STAT, (1<<MMCHS_STAT_COMMAND_COMPLETE));
    }


    /*
     * SDIO Card CHECK
     */

    // CMD 5
    while((get32(MMCHS1_PSTATE) & (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)) == (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)){
        // Line in use, wait until released
    }
    // Enable CTO, CC, CEB
    set32(MMCHS1_IE, 0x00050001);

    // Send command (CMD5) + Response Type 48 bits
    set32(MMCHS1_CMD, 0x05020000);

    delayAfterCommand();

    // Check if it is an SDIO card
    do {
        if((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_COMPLETE)) == (1<<MMCHS_STAT_COMMAND_COMPLETE)){
            // It's an SDIO card!
            // TODO: let the kernel know?
            return SDIO;
        }
    } while ((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)) != (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));

    softwareResetCMDLine();

    /*
     * SD Card v2 CHECK
     */

    // CMD 8
    while((get32(MMCHS1_PSTATE) & (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)) == (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)){
        // Line in use, wait until released
    }

    or32(MMCHS1_STAT, (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));
    // 1<<8 => voltage of 2.7-3.6 V, 1<<9=> "low voltage" check pattern 10101010
    set32(MMCHS1_ARG, 0x000001AA);

    // Enable s CERR, CIE, CCRC, CC, CTO and CEB events
    set32(MMCHS1_IE, 0x100f0001);

    // Send CMD8
    set32(MMCHS1_CMD, 0x081a0000);
    delayAfterCommand();


    // Read CC and CTO bits from from STAT to see if it is an SD card > v2
    do {
        if((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_COMPLETE)) == (1<<MMCHS_STAT_COMMAND_COMPLETE))
        {
            // SD Card > v2.
            // TODO: Handle SD card v2
            return SDv2;
        }
    } while ((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)) != (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));


    softwareResetCMDLine();

    /*
     * SD Card v1 CHECK
     */

    do{
        // CMD 55
        //sdCard_sendCommand(CMD55);
        while((get32(MMCHS1_PSTATE) & (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)) == (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE))
        {
            // Line in use, wait until released
        }
        // Enable events
        set32(MMCHS1_IE, 0x100f0001);

        or32(MMCHS1_STAT, (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));

        // Send CMD55
        set32(MMCHS1_CMD, 0x371a0000);
        delayAfterCommand();
        // CMD 55 END

        // ACMD41
        // Enable CTO, CC, CEB
        set32(MMCHS1_IE, 0x00050001);
        // Set command plus response type
        set32(MMCHS1_CMD, (0x29<<24)|(0x02<<16));


        // ACMD41 END
        delayAfterCommand();

        // Check if card is SD v1.x
        do {
            if((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_COMPLETE)) == (1<<MMCHS_STAT_COMMAND_COMPLETE))
            {
                if((get32(MMCHS1_RSP10) & (1u<<31)) != (1u<<31)){
                    // Line is busy
                    break;
                } else {
                    // Send a CMD 2 command to get information on how to access card content (CID register content)
                    set32(MMCHS1_IE, 0x00070001);
                    set32(MMCHS1_CMD, 0x02090000);
                    delayAfterCommand();


                    // Send a CMD 3 - ask the card to publish new relative card address (RCA)
                    set32(MMCHS1_IE, 0x100f0001);
                    set32(MMCHS1_ARG, 0x00000000);
                    set32(MMCHS1_CMD, 0x031a0000);
                    delayAfterCommand();

                    // Get card address from register (48 bit response)
                    uint32_t adressedCard = get32(MMCHS1_RSP10) & 0xFFFF0000; // Upper 16 + stuff bits
                    gCardAddress = (adressedCard>>16);
                    // Read card CSD Register
                    set32(MMCHS1_IE, 0x00070001);
                    set32(MMCHS1_ARG, adressedCard); // Card address (upper 16 bits) + stuff bits
                    set32(MMCHS1_CMD, 0x09090000);
                    delayAfterCommand();

                    // Send a CMD 7 = Select card (after knowing its address). Sending 0 deselects all cards.
                    // Bus in push pull mode
                    delayAfterCommand();
                    set32(MMCHS1_IE, 0x100f0001);
                    set32(MMCHS1_ARG, adressedCard);
                    set32(MMCHS1_CMD, 0x071a0000);
                    delayAfterCommand();

                    return SDv1;
                }
            }
        } while ((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)) != (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));

        softwareResetCMDLine();
    } while ((get32(MMCHS1_RSP10) & (1u<<31)) != (1u<<31));


    /*
     * MMC CHECK
     */
    sdCard_sendCommand(CMD1);
    // Check if card is MMC
    do {
        if((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_COMPLETE)) == (1<<MMCHS_STAT_COMMAND_COMPLETE))
        {
            // MMC Card
            // TODO: Handle MMC
            return MMC;
        }
    } while ((get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)) != (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT));

    // If program flow arrives here, it is an "unknown card"
    return UNDEFINED;
}

/*
 * As described in spruf98y.pdf beginning with page 3143.
 */
int32_t sdCard_initialize_Ch1(void){
    // Step 1: set clocks
    // Set internal and functional clocks for Ch1
    enableMMC1Clocks();

    // Step 2: software reset
    // Perform a software reset
    softwareReset_Ch1();

    // Step 3: set module's hardware capabilities
    initializeDefaultCapabilities_Ch1();

    // Step 4: Customize module’s idle and wake-up modes
    initializeDefaultWakeUpConfiguration_Ch1();

    controllerBusConfiguration_Ch1();

    return detectAndInitializeSdCard();
}

/*
 * A block is fixed to 512 bytes. Buffer must be of size 512. Returns how many bytes have been read (0 for error).
 */
uint32_t sdCard_read512ByteBlock(uint8_t * buffer, uint32_t address){
    // Check if dat lines are in use
    while((get32(MMCHS1_PSTATE) & (1<<MMCHS_PSTATE_COMMAND_INHIBIT_DATA_LINE)) == (1<<MMCHS_PSTATE_COMMAND_INHIBIT_DATA_LINE)){
        // DATA lines are in use
    }

    // CMD 7, select card
    set32(MMCHS1_IE, 0x100f0001);
    set32(MMCHS1_ARG, gCardAddress<<16);
    set32(MMCHS1_CMD, 0x071a0000);
    delayAfterCommand();


    // Send a CMD 16 setting block length
    set32(MMCHS1_IE, 0x100f0001);
    set32(MMCHS1_ARG, 0x00000200);
    set32(MMCHS1_CMD, 0x101a0000);
    delayAfterCommand();

    set32(MMCHS1_STAT, 0xFFFFFFFF);
    // Send a CMD 17 (one block, with data)
    // Set block size and length (512 byte block, 1 block), 512 bytes
    set32(MMCHS1_BLK, 0x00010200);
    set32(MMCHS1_ARG, address); // Set block to read (byte address!)

    // Enable interrupts
    set32(MMCHS1_IE,
          (1<<MMCHS_IE_COMMAND_COMPLETED_IE) |
          (1<<MMCHS_IE_TRANSFER_COMPLETED_IE) |
          (1<<MMCHS_IE_BUFFER_READ_READY_IE) |
          (1<<MMCHS_IE_OUT_OF_BAND_IE) |
          (1<<MMCHS_IE_COMMAND_TIMEOUT_ERROR_IE) |
          (1<<MMCHS_IE_COMMAND_CRC_ERROR_IE) |
          (1<<MMCHS_IE_COMMAND_END_BIT_ERROR_IE) |
          (1<<MMCHS_IE_COMMAND_INDEX_ERROR_IE) |
          (1<<MMCHS_IE_DATA_TIMEOUT_ERROR_IE) |
          (1<<MMCHS_IE_DATA_CRC_ERROR_IE) |
          (1<<MMCHS_IE_DATA_END_BIT_ERROR_IE) |
          (1<<MMCHS_IE_CARD_ERROR_IE) |
          (1<<MMCHS_IE_BAD_ACCESS_TO_DATA_SPACE_IE));

    set32(MMCHS1_CMD, (17<<24) | (1<<21) | (1<<20) | (1<<19) | (0x2<<16) | (0<<5) | (1<<4)| (0<<2)| (0<<1));
    delayAfterCommand();

    // Check if there was an error sending the command. If yes, return
    if((get32(MMCHS1_STAT) & (1<<15)) == (1<<15)){
        return 0;
    } else {
        // Command completed successfully. Now read data
        // Check if the Buffer Write Ready bit is set
        volatile uint32_t i = 0;
        if((REG(MMCHS1_STAT) & (1<<5)) == (1<<5)){
            for(i=0; i < SD_SECTOR_SIZE; i+=4){ // 512 bytes, 4 bytes are read
                // Read data.
                volatile uint32_t read_data = get32(MMCHS1_DATA);
                buffer[i+3] = read_data >> 24;
                buffer[i+2] = read_data >> 16;
                buffer[i+1] = read_data >> 8;
                buffer[i+0] = read_data;
                delayAfterCommand();
            }
        } else {
            // Error occured
            return 0;
        }

        // Reset buffer read ready
        or32(MMCHS1_STAT, (1<<5));
    }
    return SD_SECTOR_SIZE;
}

/*
 * Set mmci_cmd to 1 for 80 clock cycles. This is impossible, because clock cannot be set that slow. Works anyway.
 * spruf98y.pdf Page 3182
 */
void sdCard_sendInitializationSequence_Ch1(void){
    or32(MMCHS1_CON, MMCHS_HOST_TO_CARD_INIT_SEQUENCE);
}

/*
 * Sets the block count. The parameter directly represents the block number,
 * so for example passing 0xFF to this function will result in a block number
 * of 255 being set.
 */
void sdCard_setTransactionBlockCount(uint32_t blockNumber){
    // Only last 16 bits are allowed.
    uint32_t last16MSBitsBlockNumber = blockNumber << 16;

    // Write the 16 bits in position 31:16 of MMCHS1_BLK
    or32(MMCHS1_BLK, last16MSBitsBlockNumber);
}

/*
 * Sets the block size in bytes. Maximum size is 0x400 (1024 bytes).
 */
void sdCard_setTransactionBlockSize(uint32_t blockSize){
    // Make sure only the last 11 bits are written
    uint32_t last11BitsMasked = blockSize && 0x7FF;

    if(last11BitsMasked > MMCHS_1024_BYTE_BLOCK_SIZE){
        last11BitsMasked = MMCHS_1024_BYTE_BLOCK_SIZE;
    }

    or32(MMCHS1_BLK, last11BitsMasked);
}

/*
 * Send a command to an SD card. If an argument needs to be provided (no stuff bits), the
 * sdCard_sendCommandWithArgument should be used.
 */
void sdCard_sendCommand(SDCardCommands_t command){
    // Check if cmd line is in use
    while((get32(MMCHS1_PSTATE) & (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)) == (1<<MMCHS_PSTATE_COMMAND_INHIBIT_CMD_LINE)){
        // Line in use, wait until released
    }

    switch (command){
    case CMD0:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

        // Enable CC, CEB events
        set32(MMCHS1_IE, 0x00040001);

        // Send CMD0
        set32(MMCHS1_CMD, 0x00000000);
        break;
    case CMD1:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));
        set32(MMCHS1_IE, 0x00050001);
        set32(MMCHS1_CMD, 0x01020000);
        break;
    case CMD5:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

        // Enable CTO, CC, CEB
        set32(MMCHS1_IE, 0x00050001);

        // Send command (CMD5) + Response Type 48 bits
        set32(MMCHS1_CMD, 0x05020000);
        break;
    case CMD8:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

        // 1<<8 => voltage of 2.7-3.6 V, check pattern 10101010
        set32(MMCHS1_ARG, (1<<9)|(0xAA<<0));

        // Enable s CERR, CIE, CCRC, CC, CTO and CEB events
        set32(MMCHS1_IE, 0x100f0001);

        // Send CMD8
        set32(MMCHS1_CMD, 0x081a0000);
        break;
    case ACMD41:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

        // Enable CTO, CC, CEB
        set32(MMCHS1_IE, 0x00050001);

        // Set command plus response type
        set32(MMCHS1_CMD, (0x29<<24)|(0x02<<16));
        break;
    case CMD55:
        // Set open drain for broadcast (only for MMC)
        //or32(MMCHS1_CON, (1<<MMCHS_CON_OPEN_DRAIN_MODE));

        // Enable events
        set32(MMCHS1_IE, 0x100f0001);

        // Send CMD55
        set32(MMCHS1_CMD, 0x371a0000);
        break;
    default:
        break;
    }

    // CTO (timeout) ocurred
    if( (get32(MMCHS1_STAT) & (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)) == (1<<MMCHS_STAT_COMMAND_TIMEOUT_INTERRUPT)){
        softwareResetCMDLine();
    }
}
