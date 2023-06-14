#ifndef DEBUG_MINIMAL_H
#define DEBUG_MINIMAL_H

#include "avrlibdefs.h"
#include "global.h"
#include "uart.h"
#include <avr/pgmspace.h>

#ifdef DEBUG
#define DEBUG_print(s)    UART_printStr_p(PSTR(s))
#define DEBUG_println(s)  UART_printStr_p(PSTR(s "\n"))
#define DEBUG_printnum(i) UART_print_i16((int16_t)i, 0, false)
#else
#define DEBUG_print(s)
#define DEBUG_println(s)
#define DEBUG_printnum(i)
#endif
#endif /* DEBUG_MINIMAL_H */