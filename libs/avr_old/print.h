/* 
* Provides commonly used functions for dealing creating and printing strings
* 
*/

#ifndef PRINT_H
#define PRINT_H

#include "global.h"

/* stream a string from ram to provided sendChar function */
void printStr(void (*sendChar)(char), const char *s);

/* stream a string from flash to provided sendChar function */
void printStr_p(void (*sendChar)(char), const char *s);

/* create floating point string from unsigned integer and stream it to sendChar function */
void print_uint16(
    void (*sendChar)(char), // pointer to function for streaming characters
    uint16_t v,             // value
    uint8_t fp,             // number of decimal points
    uint8_t rj,             // 0 = left justified, 1 = right justfied
    uint8_t neg_sign        // 0 = no sign, 1 = negative
);

/* a wrapper for print_uint16 that handles signed integers*/
void print_int16(
    void (*sendChar)(char), // pointer to function for streaming characters
    int16_t v,              // value
    uint8_t fp,             // number of decimal points
    uint8_t rj              // 0 = left justified, 1 = right justfied
);

#endif //PRINT_H
