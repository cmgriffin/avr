/*___________________________________________________________________________________________________

Title:
        sd.h v1.0

Description:
        SD card interface using SPI

        For complete details visit:
        https://www.programming-electronics-diy.xyz/2022/07/sd-memory-card-library-for-avr.html

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

// Some bits for SPI registers
// #define SPIF0   7
// #define SPIE0   7
// #define SPR00   0
// #define SPR01   1
// #define MSTR0   4
// #define SPE0    6

// R1 Response
#define PARAM_ERROR(X)     X & 0b01000000
#define ADDR_ERROR(X)      X & 0b00100000
#define ERASE_SEQ_ERROR(X) X & 0b00010000
#define CRC_ERROR(X)       X & 0b00001000
#define ILLEGAL_CMD(X)     X & 0b00000100
#define ERASE_RESET(X)     X & 0b00000010
#define IN_IDLE(X)         X & 0b00000001

// R3 Response
#define POWER_UP_STATUS(X) X & 0x80
#define CCS_VAL(X)         X & 0x40 // Card Capacity Status
#define VDD_2728(X)        X & 0b10000000
#define VDD_2829(X)        X & 0b00000001
#define VDD_2930(X)        X & 0b00000010
#define VDD_3031(X)        X & 0b00000100
#define VDD_3132(X)        X & 0b00001000
#define VDD_3233(X)        X & 0b00010000
#define VDD_3334(X)        X & 0b00100000
#define VDD_3435(X)        X & 0b01000000
#define VDD_3536(X)        X & 0b10000000

// R7 Response
#define CMD_VER(X)        ((X >> 4) & 0xF0) // command version
#define VOL_ACC(X)        (X & 0x1F)        // voltage accepted
#define VOLTAGE_ACC_27_33 0b00000001        // 2.7-3.6V
#define VOLTAGE_ACC_LOW   0b00000010        // low voltage
#define VOLTAGE_ACC_RES1  0b00000100        // reserved
#define VOLTAGE_ACC_RES2  0b00001000        // reserved

// Data Error Token
#define SD_TOKEN_OOR(X)   X & 0b00001000 // Data Out Of Range
#define SD_TOKEN_CECC(X)  X & 0b00000100 // Card ECC Failed
#define SD_TOKEN_CC(X)    X & 0b00000010 // CC Error
#define SD_TOKEN_ERROR(X) X & 0b00000001

/* Command List */
// CMD0 - GO_IDLE_STATE
#define CMD0     0
#define CMD0_ARG 0x00000000
#define CMD0_CRC 0x94

// CMD8 - SEND_IF_COND (Send Interface Condition)
#define CMD8     8
#define CMD8_ARG 0x0000001AA
#define CMD8_CRC 0x86 // (1000011 << 1)

// CMD55 - APP_CMD
#define CMD55     55
#define CMD55_ARG 0x00000000
#define CMD55_CRC 0x00

// ACMD41 - SD_SEND_OP_COND (Send Operating Condition)
// ACMD stands for Application Specific Command and before issued, the CMD55
// must be sent first
#define ACMD41     41
#define ACMD41_ARG 0x40000000
#define ACMD41_CRC 0x00

// CMD58 - read OCR (Operation Conditions Register)
#define CMD58     58
#define CMD58_ARG 0x00000000
#define CMD58_CRC 0x00

// CMD17 - READ_SINGLE_BLOCK
// For a 16MHz oscillator and SPI clock set to divide by 2.
// Thus, to get the number of bytes we need to send over SPI to reach 100ms, we
// do (0.1s * 16000000 MHz) / (2 * 8 bytes) = 100000
#define CMD17                17
#define CMD17_CRC            0x00
#define SD_MAX_READ_ATTEMPTS (0.1 * F_CPU) / (SPI0_CLK_DIV * 8)

// CMD24 - WRITE_BLOCK
// For a 16MHz oscillator and SPI clock set to divide by 2.
// Thus, to get the number of bytes we need to send over SPI to reach 250ms, we
// do (0.25s * 16000000 MHz) / (2 * 8 bytes) = 250000
#define CMD24                 24
#define CMD24_CRC             0x00
#define SD_MAX_WRITE_ATTEMPTS (0.25 * F_CPU) / (SPI0_CLK_DIV * 8)

// Card Type
#define SD_V1_SDSC      1
#define SD_V2_SDSC      2
#define SD_V2_SDHC_SDXC 3

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

/*______________________________________________________________________________________________
        Assert Chip Select Line by sending a byte before and after changing the
CS line
_______________________________________________________________________________________________*/
static void sd_assert_cs(void);

/*______________________________________________________________________________________________
        De-Assert Chip Select Line by sending a byte before and after changing
the CS line
_______________________________________________________________________________________________*/
static void sd_deassert_cs(void);

/*______________________________________________________________________________________________
        Send ACMD41 - Sends host capacity support information and activates the
card's initialization process. Reserved bits shall be set to '0'.
_______________________________________________________________________________________________*/
static SD_RETURN_CODES sd_command_ACMD41(void);

/*______________________________________________________________________________________________
        Wait for a response from the card other than 0xFF which is the normal
state of the MISO line. Timeout occurs after 16 bytes.

        return		response code
_______________________________________________________________________________________________*/
static uint8_t sd_read_response1(void);

/*______________________________________________________________________________________________
        Read Response 3 or 7 which is 5 bytes long

        res			Pointer to a 5 bytes array
_______________________________________________________________________________________________*/
static void sd_read_response3_7(uint8_t *res);

/*______________________________________________________________________________________________
        Send a command to the card

        cmd			8-bit command index
        arg			a 32-bit argument
        crc			an 8-bit CRC
_______________________________________________________________________________________________*/
static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc);

// static void SPI_Init(void);
// void SPI_Send(uint8_t *buf, uint16_t length);
// static void SPI_SendByte(uint8_t byte);
// void SPI_Receive(uint8_t *buf, uint16_t length);
// static uint8_t SPI_ReceiveByte(void);

#endif /* SD_H_ */