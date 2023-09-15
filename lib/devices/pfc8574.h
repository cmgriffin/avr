#ifndef PFC8574_H
#define PFC8574_H
#include "avrlibdefs.h"
/**
 * @brief Write to the output port
 * Setting a bit produces a pullup high, clearing a bit produces a strong
 * pulldown
 *
 * @param addr 7-bit style address
 * @param data data to be written to the port
 */
void PFC8574_write(uint8_t addr, uint8_t data);

/**
 * @brief Read from the output port
 *
 * @param addr 7-bit style address
 * @return uint8_t data read from the port
 */
uint8_t PFC8574_read(uint8_t addr);

#endif /* PFC8574_H */