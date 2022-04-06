#ifndef __LM75__
#define __LM75__

#include "avrlibdefs.h"

#define LM75_ADDR_BASE 0x48

#define LM75_REG_TEMP  0x0
#define LM75_REG_CONF  0x1
#define LM75_REG_THYST 0x2
#define LM75_REG_TOS   0x3

int8_t LM75_measure_temp_8b(uint8_t addr);

int16_t LM75_measure_temp_9b(uint8_t addr);

#endif // __LM75__