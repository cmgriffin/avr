/**
 * @file stream.h
 * @author your name (you@domain.com)
 * @brief A library of generic functions for sending and recieving character
 * streams
 * @version 0.1
 * @date 2022-02-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief a struct to hold the necessary function pointers for sending and
 * receiving bytes
 * Each function returns a bool indicating if the operation was a success.
 * A bool is also passed in indicating if the function call should be blocking
 * The tx function is passed byte to be sent. The rx function is passed pointer
 * to a byte to store the recieved value
 */
typedef struct
{
    bool (*tx_func)(uint8_t, bool);
    bool (*rx_func)(uint8_t *, bool);
} stream_t;

/**
 * @brief Initalizer for the stream
 *
 */
#define STREAM_CREATE(TX_FUNC, RX_FUNC)        \
    {                                          \
        .tx_func = TX_FUNC, .rx_func = RX_FUNC \
    }

/**
 * @brief A generic putChar function for use with stdio functions
 *
 * @param c character to send
 * @param io struct of pointers to tx and rx funtions
 * @param stream stdio FILE type stream
 * @return int result of operation
 */
int STREAM_putChar(char c, stream_t *io, FILE *stream);

/**
 * @brief A generic getChar function for use with stdio functions
 *
 * @param s char buffer of size n
 * @param n size of char buffer
 * @param rxptr pointer to current address in the buffer
 * @param io struct of pointers to tx and rx funtions
 * @param stream stdio FILE type stream
 * @return int result of operation
 */
int STREAM_getChar(char *s, uint8_t n, char **rxptr, stream_t *io, FILE *stream);

/**
 * @brief A non-blocking readline routine
 * Each call chars available will be read in and stored in s. Until a newline is
 * encountered NULL will be returned. Once newline is recieved a pointer to the
 * buffer will be returned *
 *
 * @param s char buffer of size n
 * @param n size of char buffer
 * @param rxptr pointer to current address in the buffer
 * @param io struct of pointers to tx and rx funtions
 * @return char* pointer of buffer when ready or NULL otherwise
 */
char *STREAM_readLine(char *s, uint8_t n, char **rxptr, stream_t *io);

/**
 * @brief Basic routine for printing a string stored in RAM
 *
 * @param s char string stored in RAM
 * @param io struct of pointers to tx and rx funtions
 */
void STREAM_printStr(const char *s, stream_t *io);

/**
 * @brief Basic routine for printing a string stored in FLASH
 *
 * @param s char string stored in FLASH
 * @param io struct of pointers to tx and rx funtions
 */
void STREAM_printStr_p(const char *s, stream_t *io);

/**
 * @brief Routine to print unsigned integers
 *
 * @param v integer value to be printed
 * @param fp number of digits after the decimal point
 * @param rj right justify the output
 * @param neg_sign add a negative sign
 * @param io struct of pointers to tx and rx funtions
 */
void STREAM_print_u16(uint16_t v, uint8_t fp, bool rj, bool neg_sign,
                      stream_t *io);

/**
 * @brief Routine to print signed integers
 *
 * @param v integer value to be printed
 * @param fp number of digits after the decimal point
 * @param rj right justify the output
 * @param io struct of pointers to tx and rx funtions
 */
void STREAM_print_i16(int16_t v, uint8_t fp, bool rj, stream_t *io);

#endif /* STREAM_H */
