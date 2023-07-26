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
static inline void DEBUG_printint(int16_t i)
{
    char s[7];
    UART_printStr(itoa(i, s, 10));
}
static inline void DEBUG_printlong(int32_t l)
{
    char s[12];
    UART_printStr(ltoa(l, s, 10));
}
#define DEBUG_printnum(i) DEBUG_printlong(i)
#else
#define DEBUG_print(s)
#define DEBUG_println(s)
#define DEBUG_printnum(i)
#define DEBUG_printint(i)
#define DEBUG_printlong(i)
#endif
#endif /* DEBUG_MINIMAL_H */