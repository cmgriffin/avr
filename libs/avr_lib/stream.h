/**
 * @file stream.h
 * @author your name (you@domain.com)
 * @brief A library of generic functions for sending and recieving character streams
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
 *
 */
typedef struct
{
    void (*tx_func)(uint8_t);
    uint8_t (*rx_func)(bool);
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
 * @brief Generic putChar function for stdio use
 * Redundant looking parameters are the result of stdio functions requiring
 * a FILE *stream but we cannot use it externally
 *
 * @param c
 * @param transmitByte_func
 * @return int
 */
int STREAM_putChar(char c, stream_t *io, FILE *stream);

/**
 * @brief A blocking line editor style buffered getChar function for stdio
 *
 *
 *
 * @param s
 * @param stream
 * @return int
 */
int STEAM_getChar(stream_t *io, FILE *stream);

/**
 * @brief A non-blocking line read routine
 *
 * @param b* pointer to string for holding the line
 * @param s stream_t struct
 * @return char* NULL pointer if newline not found else pointer to b
 */
char *STREAM_getLine(uint8_t *s, stream_t *io);

#endif /* STREAM_H */
