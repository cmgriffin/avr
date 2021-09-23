#include <avr\pgmspace.h>
#include "print.h"

const uint16_t PROGMEM powers_of_ten[] = {0, 1, 10, 100, 1000, 10000};

void printStr(void (*sendChar)(char), const char *s)
{
	while (*s)
	{
		if (*s == '\n')
			sendChar('\r');
		sendChar(*s++);
	}
}

void printStr_p(void (*sendChar)(char), const char *s)
{
	char c;

	for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
	{
		if (c == '\n')
			sendChar('\r');
		sendChar(c);
	}
}

void print_uint16(void (*sendChar)(char), uint16_t v, uint8_t fp, uint8_t rj, uint8_t neg_sign)
{
	/*
	* a total of 5 digits implemented due to 65,536 being max for unsigned integer

	*/
	uint8_t first_digit = 1;
	if (rj && !neg_sign)
	{
		sendChar(' ');
	}
	for (uint8_t pos = 5; pos > 0; pos--)
	{

		uint16_t n = (v / pgm_read_word(&powers_of_ten[pos])) % 10;

		if (pos == fp)
		{ // separate conditional since the decimal point gets printed right before the selected character
			if (first_digit)
			{
				if (neg_sign)
				{
					sendChar('-');
				}
				sendChar('0');
				first_digit = 0;
			}
			sendChar('.');
		}
		if ((n != 0) && (first_digit))
		{
			if (neg_sign)
			{
				sendChar('-');
			}
			sendChar('0' + n);
			first_digit = 0;
		}
		else if (rj && first_digit && !((pos - 1) == fp))
		{
			sendChar(' ');
		}
		else if (!first_digit)
		{
			sendChar('0' + n);
		}
	}
}

void print_int16(void (*sendChar)(char), int16_t v, uint8_t fp, uint8_t rj)
{
	if (v < 0)
	{
		v *= -1;
		print_uint16(sendChar, v, fp, rj, 1);
	}
	else
	{
		print_uint16(sendChar, v, fp, rj, 0);
	}
}