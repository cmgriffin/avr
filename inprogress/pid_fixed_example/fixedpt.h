#ifndef FIXEDPT_H
#define FIXEDPT_H

#include <stdint.h>

typedef int16_t Q7_8;
typedef int32_t Q15_16;

#define FIXEDPT_BITS     8

#define FIXEDPT_CONST(n) ((Q7_8)(n * (1 << FIXEDPT_BITS)))
#define FIXEDPT_MULT(a, b)                                         \
    ((Q7_8)(((Q15_16)a * (Q15_16)b + (1 << (FIXEDPT_BITS - 1))) >> \
            FIXEDPT_BITS))
#define FIXEDPT_DIV(a, b) \
    ((Q7_8)(((((Q15_16)a) << FIXEDPT_BITS) + ((Q15_16)b >> 1)) / ((Q15_16)b)))

#endif /* FIXEDPT_H */