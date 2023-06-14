/*___________________________________________________________________________________________________

Title:
        sd.h v1.0

Description:
        SD card interface using SPI

        For complete details visit:
        https://www.programming-electronics-diy.xyz/2022/07/sd-memory-card-library-for-avr.html
        http://www.rjhcoding.com/avrc-sd-interface-1.php

Author:
        Liviu Istrate
        istrateliviu24@yahoo.com
        www.programming-electronics-diy.xyz

Donate:
        Software development takes time and effort so if you find this useful
consider a small donation at: paypal.me/alientransducer
_____________________________________________________________________________________________________*/

/* ----------------------------- LICENSE - GNU GPL v3
-----------------------------------------------

* This license must be included in any redistribution.

* Copyright (C) 2022 Liviu Istrate, www.programming-electronics-diy.xyz
(istrateliviu24@yahoo.com)

* Project URL:
https://www.programming-electronics-diy.xyz/2022/07/sd-memory-card-library-for-avr.html
* Inspired by: http://www.rjhcoding.com/avrc-sd-interface-1.php

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE
--------------------------------------------------*/

#ifndef SD_H_
#define SD_H_

#include <avrlibdefs.h>

// /*************************************************************
// 	USER DEFINED SETTINGS
// **************************************************************/
// // SPI0 I/O pins (MOSI and SCK)
// #define SPI0_PORT PORTB
// // SPI PORT #define SPI0_DDR DDRB  // SPI DDR #define SPI0_PINS PINB  //
// Holds the state of each pin on port where SPI pins are

// #define SPI0_MOSI_PIN PB5
// // SDA pin
// #define SPI0_SCK_PIN							PB7   //
// SCK pin

// // SPI Chip Select pin
// #define CARD_CS_DDR DDRB #define CARD_CS_PORT
// PORTB #define CARD_CS_PIN
// PB4

// // Card detect pin for card sockets with this feature (optional)
// #define CARD_DETECT_DDR DDRB #define CARD_DETECT_PORT
// PORTB #define CARD_DETECT_PIN
// PB1 #define CARD_DETECT_PINS						PINB

#define SPI0_CLK_DIV 2 // smallest SPI prescaler gives highest speed

/*************************************************************
        SYSTEM DEFINES
**************************************************************/
// Block length can be set in Standard Capacity SD cards using CMD16
// (SET_BLOCKLEN). For SDHC and SDXC cards the block length is always set to 512
// bytes.
#define SD_BUFFER_SIZE 512 // CAUTION: only 512 bytes/block is implemented

/*************************************************************
        GLOBALS
**************************************************************/
extern uint8_t SD_Buffer[SD_BUFFER_SIZE + 1]; // reserve 1 byte for the null
extern uint8_t SD_ResponseToken;
// Sectors below this value will not be written by the write functions.
// Protected sectors could include Boot Sectors, FATs, Root Directory.
// uint16_t SD_MaxProtectedSector;

/* Card initialization return codes (SD_RETURN_CODES) */
typedef enum
{
    SD_OK,
    SD_IDLE_STATE_TIMEOUT,
    SD_GENERAL_ERROR,
    SD_CHECK_PATTERN_MISMATCH,
    SD_NONCOMPATIBLE_VOLTAGE_RANGE,
    SD_POWER_UP_BIT_NOT_SET,
    SD_NOT_SD_CARD,
    // SD_OP_COND_TIMEOUT,
    // SD_SET_BLOCKLEN_TIMEOUT,
    // SD_WRITE_BLOCK_TIMEOUT,
    // SD_WRITE_BLOCK_FAIL,
    // SD_READ_BLOCK_TIMEOUT,
    // SD_READ_BLOCK_DATA_TOKEN_MISSING,
    // SD_DATA_TOKEN_TIMEOUT,
    // SD_SELECT_CARD_TIMEOUT,
    // SD_SET_RELATIVE_ADDR_TIMEOUT
} SD_RETURN_CODES;

/*************************************************************
        FUNCTION PROTOTYPES
**************************************************************/

/*______________________________________________________________________________________________
        Initialize the SD card into SPI mode
        returns 0 on success or error code
________________________________________________________________________________________________*/
SD_RETURN_CODES sd_init(void);

/*______________________________________________________________________________________________
        Write a single block of data

        addr	32-bit address. SDHC and SDXC are block addressed, meaning an
address of 0 will read back bytes 0-511, address 1 will read back 512-1023, etc.
                        With 32 bits to specify a 512 byte block, the maximum
size a card can support is (2^32)*512 = 2048 megabytes or 2 terabytes.

        buff	512 bytes of data to write

        token	0x00 - busy timeout
                        0x05 - data accepted
                        0xFF - response timeout
_______________________________________________________________________________________________*/
uint8_t sd_write_single_block(uint32_t addr, uint8_t *buf);

/*______________________________________________________________________________________________
        Read a single block of data

        addr	32-bit address. SDHC and SDXC are block addressed, meaning an
address of 0 will read back bytes 0-511, address 1 will read back 512-1023, etc.
                        With 32 bits to specify a 512 byte block, the maximum
size a card can support is (2^32)*512 = 2048 megabytes or 2 terabytes.

        buff	a buffer of at least 512 bytes to store the data in

        token	0xFE - Successful read
                        0x0X - Data error (Note that some cards don't return an
error token instead timeout will occur) 0xFF - Timeout
_______________________________________________________________________________________________*/
uint8_t sd_read_single_block(uint32_t addr, uint8_t *buf);

#endif /* SD_H_ */