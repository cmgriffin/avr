/* 
* Provides commonly used functions for dealing with the UART
* 
*/
#ifndef UART_H
#define UART_H

#ifndef BAUD
#define BAUD 9600
#endif

#include <avr/pgmspace.h>
#include <stdint.h>

#include "print.h"

#define BUFFER_SIZE 20

extern char rxbuff[];
extern volatile uint8_t rx_rdy;

void UART_init(void);

void UART_sendChar(char c);

char UART_getChar(void);

void UART_flush(void);

static inline void UART_printStr(const char *s)
{
	printStr(UART_sendChar, s);
}

static inline void UART_printStr_p(const char *s)
{
	printStr_p(UART_sendChar, s);
}

void UART_readStr(char s[], uint8_t maxLength);

#endif //UART_H
