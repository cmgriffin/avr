/**
 * @file conv2str.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CONV2STR_H
#define CONV2STR_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

char *CONV2STR_fptostr31(char *s, int16_t v, uint8_t fp);
// char *CONV2STR_fptostr31(char *s, int16_t v);

char *CONV2STR_fptostr31rj(char *s, int16_t v, uint8_t fp);

char *CONV2STR_fptostrU16(char *s, uint16_t v, uint8_t fp, bool rj,
                          bool neg_sign);

char *CONV2STR_fptostrI16(char *s, uint16_t v, uint8_t fp, bool rj);

#endif // CONV2STR