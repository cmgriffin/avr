/**
 * @file uart.h
 * @author C. Griffin
 * @brief Module provides some commonly used routines for working with the UART
 * @version 0.1
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef UART_H
#define UART_H

#include "global.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Initialize the UART peripheral
 * Performs the following initializations
 * - configures the baud rate for the defined value
 * - enables the transmitter and receiver
 * - enables tx and/or rx interupts if defined
 * - defines stdout and/or stdin if defined
 *
 */
void UART_init(void);

/**
 * @brief Low level function to transmit a byte using the UART
 *
 * @param c byte to transmit
 * @param blocking config if routine should block execution if unable to
 * immediately transmit the data
 * @return true transmition of byte was successful
 * @return false transmition of byte was not successful
 * e.g. peripheral was not ready and function was not configured to be blocking
 */
bool UART_TransmitByte(uint8_t c, bool blocking);

/**
 * @brief Low level function to receive a byte using the UART
 *
 * @param c pointer to var where byte is to be recieved into
 * @param blocking config if routine should block execution if unable to
 * immediately receive the data
 * @return true receive of byte was successful
 * @return false receive of byte was not successful
 * e.g. no data was available and function was not configured to be blocking
 */
bool UART_ReceiveByte(uint8_t *c, bool blocking);

/**
 * @brief Return the number of bytes waiting in rx buffer
 *
 * @return uint8_t number of bytes waiting in rx buffer
 */
uint8_t UART_available();

/**
 * @brief A UART putChar function for use with stdio functions
 *
 * @param c character to send
 * @param stream stdio FILE type stream
 * @return int result of operation
 */
int UART_putChar(char c, FILE *stream);

/**
 * @brief A UART getChar function for use with stdio functions
 *
 * @param stream stdio FILE type stream
 * @return int result of operation
 */
int UART_getChar(FILE *stream);

/**
 * @brief Discard all data in UART recieve buffer
 * TODO: make this more generic and make it work with buffered reads
 *
 */
void UART_flush(void);

/**
 * @brief A non-blocking readline routine
 * Each call chars available will be read in and stored in s. Until a newline is
 * encountered NULL will be returned. Once newline is recieved a pointer to the
 * buffer will be returned *
 *
 * @param s char buffer of size n
 * @param n size of char buffer
 * @param rxptr pointer to current address in the buffer
 * @return char* pointer of buffer when ready or NULL otherwise
 */
char *UART_readLine(char *s, uint8_t n, char **rxptr);

/**
 * @brief Basic routine for printing a string stored in RAM
 *
 * @param s char string stored in RAM
 */
void UART_printStr(const char *s);

/**
 * @brief Basic routine for printing a string stored in FLASH
 *
 * @param s char string stored in FLASH
 */
void UART_printStr_p(const char *s);

/**
 * @brief Routine to print unsigned integers
 *
 * @param v integer value to be printed
 * @param fp number of digits after the decimal point
 * @param rj right justify the output
 * @param neg_sign add a negative sign
 */
void UART_print_u16(uint16_t v, uint8_t fp, bool rj, bool neg_sign);

/**
 * @brief Routine to print signed integers
 *
 * @param v integer value to be printed
 * @param fp number of digits after the decimal point
 * @param rj right justify the output
 */
void UART_print_i16(uint16_t v, uint8_t fp, bool rj);

#endif // UART_H
