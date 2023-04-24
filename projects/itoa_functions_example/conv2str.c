#include "conv2str.h"

int16_t __intpow5(int16_t exp)
{
    int16_t result = 1;
    int16_t base   = 5;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

int16_t __fp2int(int16_t v, uint8_t fp, uint8_t decimals)
{
    // v *= 5; // fixed for number of decimal pts
    v *= __intpow5(5);

    // addition for integer rounding
    // v += (fp > 1) ? 1 << (fp - decimals - 1) : 0;
    v += (fp > (decimals - 1)) ? 1 << (fp - decimals - 1) : 0;

    // right shift
    // v >>= (fp > 0) ? (fp - 1) : 0;

    if (fp >= decimals)
    {
        v >>= fp - decimals;
    }
    else
    {
        v <<= decimals - fp;
    }
    return v;
}

/**
 * @brief
 *
 * Concept:
 * - multiply starting value by 5 ^ (fp bits right of decimal point)
 *      this creates a complete integer version of the value with all
 *      decimals shifted accordingly
 * - divide by 10 to removed digits that are not going to be printed
 *
 * This simplifies to:
 * - mutliply starting value by 5 ^ (display decimals)
 * - add 2^(fp bits - display decimals - 1) to preserve proper rounding
 * - right shift value by (fp bits - display decimals)
 *
 * @param s
 * @param v
 * @param fp
 * @return char*
 */

// char *CONV2STR_fptostr31(char *s, int16_t v, uint8_t fp)
// {

//     const uint8_t decimal_places = 1; //

//     s[0]                         = (v >= 0) ? '+' : '-';
//     v                            = __fp2int(v, fp, 1);
//     s[1]                         = (v / 1000) % 10 + '0';
//     s[2]                         = (v / 100) % 10 + '0';
//     s[3]                         = (v / 10) % 10 + '0';
//     s[4]                         = '.';
//     s[5]                         = (v) % 10 + '0';
//     s[6]                         = '\0';
//     return s;
// }

#define FP 4

char *CONV2STR_fptostr31(char *s, int16_t v)
{

    const uint8_t decimals = 1; //

    s[0]                   = (v >= 0) ? '+' : '-';
    // v *= 5; // fixed for number of decimal pts
    v *= __intpow5(5);

    // addition for integer rounding
    // v += (fp > 1) ? 1 << (fp - decimals - 1) : 0;
    v += (FP > (decimals - 1)) ? 1 << (FP - decimals - 1) : 0;

    // right shift
    // v >>= (fp > 0) ? (fp - 1) : 0;

    if (FP >= decimals)
    {
        v >>= FP - decimals;
    }
    else
    {
        v <<= decimals - FP;
    }

    s[1] = (v / 1000) % 10 + '0';
    s[2] = (v / 100) % 10 + '0';
    s[3] = (v / 10) % 10 + '0';
    s[4] = '.';
    s[5] = (v) % 10 + '0';
    s[6] = '\0';
    return s;
}