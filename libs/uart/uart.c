#include <avr/interrupt.h>
#include <util/setbaud.h>
#include "uart.h"
#include "print.h"

char rxbuff[20];
volatile uint8_t rxbuff_pos = 0;
volatile uint8_t rx_rdy = 0;

void UART_init(void)
{
	UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
	/* Enable USART transmitter/receiver and receiver interupt*/
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */
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
	unsigned char dummy;
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

ISR(USART_RX_vect)
{
	volatile char response;
	response = UDR0;
	if (bit_is_clear(UCSR0A, FE0)) // if no framing error occurred
	{
		if (response == '\r')
		{
			response = '\n';
		}
		// a return indicates the end of a command
		if (response == '\n')
		{
			UART_sendChar(response); // echo the character back
			rx_rdy = 1;				 // set the flag to alert the main loop
			rxbuff[rxbuff_pos] = 0;	 // add the null character to the end
			rxbuff_pos = 0;			 // reset the buffer position to start
			//disable interrupts otherwise remaining chars in hardware buffer will retrigger ISR
			UCSR0B &= ~(1 << RXCIE0);
		}
		else if (rxbuff_pos < BUFFER_SIZE - 1)
		{
			UART_sendChar(response); // echo the character back
			rxbuff[rxbuff_pos] = response;
			rxbuff_pos++;
		}
	}
}