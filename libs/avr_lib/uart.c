#include "uart.h"
#include "buffer.h"
#include "global.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>

#ifdef UART_RX_INTERUPT
static uint8_t _rxbuff[UART_RX_BUFFER_SIZE];
static buffer_t rxbuff = BUFFER_CREATE(UART_RX_BUFFER_SIZE, _rxbuff) volatile uint8_t rxbuff_pos = 0;
static volatile uint8_t rx_rdy = 0;
#endif

#ifdef UART_INIT_STDOUT
static FILE uart_stream = FDEV_SETUP_STREAM(UART_putChar, NULL, _FDEV_SETUP_WRITE);
#endif

void UART_init(void)
{
	// set up the baud rate
	UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

	/* Enable USART transmitter/receiver and receiver interupt*/
#ifdef UART_RX_INTERUPT
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
#else
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
#endif
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */

#ifdef UART_INIT_STDOUT
	stdout = &uart_stream;
#endif
#ifdef UART_INIT_STDERR
	stderr = &uart_stream;
#endif
}

int UART_putChar(char c, FILE *stream)
{
	if (c == '\n')
		UART_putChar('\r', stream);
	/* Wait for empty transmit buffer */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c; /* send data */
	return 0;
}

void UART_sendChar(char c)
{
	/* Wait for empty transmit buffer */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c; /* send data */
}

char UART_getChar(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait for incoming data */
	return UDR0;						 /* return register value */
}

void UART_flush(void)
{
	uint8_t dummy __attribute__((unused));
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

void UART_readString(char *s, uint8_t maxLength, uint8_t echo)
{
	uint8_t i = 0;
	char c;

	while ((c = UART_getChar()) != '\n')
	{
		if (i < maxLength - 2)
		{
			if (echo)
				UART_sendChar(c);
			s[i] = c;
			i++;
		}
	}
	if (echo)
		UART_sendChar(c);
	UART_flush();
	s[i] = 0;
}

#ifdef UART_RX_INTERUPT

uint8_t UART_available()
{
	return rxbuff.num_entries;
}

uint8_t UART_getBufChar()
{
	return BUFFER_num_entries(&rxbuff);
}

ISR(USART_RX_vect, ISR_BLOCK ISR_FLATTEN)
{
	sei() volatile uint8_t response;
	response = UDR0;
	if (bit_is_clear(UCSR0A, FE0)) // if no framing error occurred
	{
		if (response == '\r')
		{
			BUFFER_enqueue(&rxbuff, response);
		}
	}
}
#endif