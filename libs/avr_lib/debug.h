#ifndef __DEBUG__
#define __DEBUG__

#include <stdio.h>
#include <avr/pgmspace.h>

#ifdef DEBUG
#define _DEBUG(fmt, args...)                                                                   \
    {                                                                                          \
        printf_P(PSTR("[DEBUG]:%s:%s:%d: " fmt "\n"), __FILE__, __FUNCTION__, __LINE__, args); \
    }
#else
#define _DEBUG(fmt, args...) \
    {                        \
    }
#endif

#endif // __DEBUG__