#ifndef DEBUG_MINIMAL_H
#define DEBUG_MINIMAL_H

#include "avrlibdefs.h"
#include "global.h"
#include "uart.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

#ifdef DEBUG
#define DEBUG_print(s)   UART_printStr_p(PSTR(s))
#define DEBUG_println(s) UART_printStr_p(PSTR(s "\n"))
static inline void DEBUG_printnum(int16_t i)
{
    char s[10];
    UART_printStr(itoa(i, s, 10));
}
#else
#define DEBUG_print(s)
#define DEBUG_println(s)
#define DEBUG_printnum(i)
#endif
#endif /* DEBUG_MINIMAL_H */