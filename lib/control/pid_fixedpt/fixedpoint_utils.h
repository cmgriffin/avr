#ifndef __FIXEDPOINT_UTILS__
#define __FIXEDPOINT_UTILS__

#include "fixedpt_conf.h"
#include <stdint.h>

/**
 * @brief Convert fixed point int to float
 *
 */
#define TO_FLOAT(n) ((double)(n) / (1 << SCALE_FACTOR))

/**
 * @brief Convert float variable to fixed point int
 * Do not use for constant values as it may invoke floating point libraries
 *
 */
#define TO_INT(n) ((int16_t)((double)(n) * (1 << SCALE_FACTOR)))

/**
 * @brief Convert float constant to fixed point int
 *
 */
#define FIXEDPT_CONST(n) ((int16_t)((n) * (1 << SCALE_FACTOR) + 0.5))

/**
 * @brief Multiply fixed point int a with fixed point int b
 *
 */
#define FIXEDPT_MULT(a, b)                                              \
    ((int16_t)(((int32_t)a * (int32_t)b + (1 << (SCALE_FACTOR - 1))) >> \
               SCALE_FACTOR))

/**
 * @brief Divide fixed point int a by fixed point int b
 *
 */
#define FIXEDPT_DIV(a, b)                                             \
    ((int16_t)(((((int32_t)a) << SCALE_FACTOR) + ((int32_t)b >> 1)) / \
               ((int32_t)b)))

#endif // __FIXEDPOINT_UTILS__