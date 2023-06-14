#include "conv2str.h"

int16_t __intpow(int16_t exp, int16_t base)
{
    int16_t result = 1;
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

/**
 * @brief an efficient integer to power of 5 function
 */
int16_t __intpow5(int16_t exp)
{
    const int16_t base = 5;
    return __intpow(exp, base);
}

/**
 * @brief an efficient integer to power of 10 function
 */
int16_t __intpow10(int16_t exp)
{
    const int16_t base = 10;
    return __intpow(exp, base);
}

/**
 * @brief convert a fixed point number of equivalent decimnal number
 *
 * @param v fixed point integer value
 * @param fp number of bits used for fractional part of fixed point
 * @param decimals number of decimal places to be preserved
 * @return int16_t decimal number multiplied by 10^decimals
 */
int16_t __fp2int(int16_t v, uint8_t fp, uint8_t decimals)
{
    // v *= 5; // fixed for number of decimal pts
    v *= __intpow5(decimals);

    // addition for integer rounding
    // v += (fp > 1) ? 1 << (fp - decimals - 1) : 0;
    v += (fp > (decimals)) ? 1 << (fp - decimals - 1) : 0;

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

char *CONV2STR_fptostr31(char *s, int16_t v, uint8_t fp)
{

    const uint8_t decimal_places = 1; //

    s[0]                         = (v >= 0) ? '+' : '-';
    v                            = __fp2int(v, fp, decimal_places);
    s[1]                         = (v / 1000) % 10 + '0';
    s[2]                         = (v / 100) % 10 + '0';
    s[3]                         = (v / 10) % 10 + '0';
    s[4]                         = '.';
    s[5]                         = (v) % 10 + '0';
    s[6]                         = '\0';
    return s;
}

char *CONV2STR_fptostr31rj(char *s, int16_t v, uint8_t fp)
{

    const uint8_t decimal_places = 1; //

    // s[0]                         = (v >= 0) ? '+' : '-';
    v    = __fp2int(v, fp, decimal_places);
    s[0] = v >= 1000 ? (v / 1000) % 10 + '0' : ' ';
    s[1] = v >= 100 ? (v / 100) % 10 + '0' : ' ';
    s[2] = (v / 10) % 10 + '0';
    s[3] = '.';
    s[4] = v % 10 + '0';
    s[5] = '\0';
    return s;
}

// #define FP 4

// char *CONV2STR_fptostr31(char *s, int16_t v)
// {

//     const uint8_t decimals = 1; //

//     s[0]                   = (v >= 0) ? '+' : '-';
//     // v *= 5; // fixed for number of decimal pts
//     v *= __intpow5(5);

//     // addition for integer rounding
//     // v += (fp > 1) ? 1 << (fp - decimals - 1) : 0;
//     v += (FP > (decimals - 1)) ? 1 << (FP - decimals - 1) : 0;

//     // right shift
//     // v >>= (fp > 0) ? (fp - 1) : 0;

//     if (FP >= decimals)
//     {
//         v >>= FP - decimals;
//     }
//     else
//     {
//         v <<= decimals - FP;
//     }

//     s[1] = (v / 1000) % 10 + '0';
//     s[2] = (v / 100) % 10 + '0';
//     s[3] = (v / 10) % 10 + '0';
//     s[4] = '.';
//     s[5] = (v) % 10 + '0';
//     s[6] = '\0';
//     return s;
// }

char *CONV2STR_fptostrU16(char *s, uint16_t v, uint8_t fp, bool rj,
                          bool neg_sign)
{
    /*
    * a total of 5 digits implemented due to 65,536 being max for unsigned
    integer

    */
    uint8_t first_digit = 1;
    char *p             = s;
    if (rj && !neg_sign)
    {
        *p++ = ' ';
    }
    for (uint8_t pos = 5; pos <= 5; pos--)
    {

        uint16_t n = (v / __intpow10(pos)) % 10;

        if (pos == fp)
        { // separate conditional since the decimal point gets printed right
          // before the selected character
            if (first_digit)
            {
                if (neg_sign)
                {
                    *p++ = '-';
                }

                *p++        = '0';
                first_digit = 0;
            }

            *p++ = '.';
        }
        if ((n != 0) && (first_digit))
        {
            if (neg_sign)
            {
                *p++ = '-';
            }

            *p++        = '0' + n;
            first_digit = 0;
        }
        else if (rj && first_digit && !((pos - 1) == fp))
        {
            *p++ = ' ';
        }
        else if (!first_digit)
        {
            *p++ = '0' + n;
        }
    }
    *p++ = '\0';
    return s;
}

char *CONV2STR_fptostrI16(char *s, uint16_t v, uint8_t fp, bool rj)
{
    bool neg_sign = false;
    if (v < 0)
    {
        v *= -1;
        neg_sign = true;
    }
    return CONV2STR_fptostrU16(s, v, fp, rj, neg_sign);
}