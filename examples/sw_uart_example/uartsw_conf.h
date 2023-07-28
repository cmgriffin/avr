/*! \file uartswconf.h \brief Interrupt-driven Software UART Driver
 * Configuration. */
//*****************************************************************************
//
// File Name	: 'uartswconf.h'
// Title		: Interrupt-driven Software UART Driver Configuration
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 7/20/2002
// Revised		: 4/27/2004
// Version		: 0.1
// Target MCU	: Atmel AVR Series (intended for the ATmega16 and ATmega32)
// Editor Tabs	: 4
//
// Description	:
//		This uart library emulates the operation of a UART (serial port)
// using 	the AVR's hardware timers, I/O pins, and some software.
//
//	Specifically, this code uses:
//		-Timer 1 Output Compare A for transmit timing
//		-Timer 1 Output Compare B for receive timing
//		-Timer 1 Input Capture for receive triggering
//
//	The above resources cannot be used for other purposes while this
// software 	UART is enabled.  The overflow interrupt from Timer1 can still
// be used for 	other timing, but the prescaler for Timer1 must not be changed.
//
//	Serial output from this UART can be routed to any I/O pin.  Serial input
//	for this UART must come from the Timer1 Input Capture (IC1) I/O pin.
//	These options should be configured by editing your local copy of
//	"uartswconf.h".
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef __UARTSW_CONF__
#define __UARTSW_CONF__

// constants/macros/typdefs
#define UARTSW_BAUD           9600

#define UARTSW_RX_BUFFER_SIZE 0x10 ///< UART receive buffer size in bytes

// #define UARTSW_INVERT					///< define to
// invert polarity of RX/TX signals when non-inverted, the serial line is
// appvropriate for passing though an RS232 driver like the MAX232.  When
// inverted, the serial line can directly drive/receive RS232 signals to/from a
// DB9 connector. Be sure to use a current-limiting resistor and perhaps a
// diode-clamp circuit when connecting incoming RS232 signals to a
// microprocessor I/O pin.

// if non-inverted, the serial line idles high (logic 1) between bytes
// if inverted, the serial line idles low (logic 0) between bytes

// UART transmit pin defines
#define UARTSW_TX_PIN GPIO_D10 ///< UART Transmit Port

// UART receive pin defines
// This pin must correspond to the
// Timer1 Input Capture (ICP or IC1) pin for your processor
#define UARTSW_RX_PIN GPIO_D8

#endif // __UARTSW_CONF__