#ifndef DEBUG_H
#define DEBUG_H

#include "global.h"
#include <avr/pgmspace.h>
#include <stdio.h>

#ifdef DEBUG
#define _DEBUG(fmt, args...)                                                  \
    {                                                                         \
        printf_P(PSTR("[DEBUG]:%s:%s:%d: " fmt "\n"), __FILE__, __FUNCTION__, \
                 __LINE__, args);                                             \
    }
#elif defined(DEBUG_SHORT)
#define _DEBUG(fmt, args...)                        \
    {                                               \
        printf_P(PSTR("[DEBUG]: " fmt "\n"), args); \
    }
#else
#define _DEBUG(fmt, args...) \
    {                        \
    }
#endif

#endif /* DEBUG_H */