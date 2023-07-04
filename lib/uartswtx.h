/**
 * @file uartswtx.h
 * @author your name (you@domain.com)
 * @brief A basic TX only UART for debugging that requires no interupts
 * @version 0.1
 * @date 2023-07-04
 *
 * @copyright Copyright (c) 2023
 *
 * This should not be used for critical communications as there is no garantee
 * an interupt will not occur and disrupt the bit timing.
 *
 *
 */
#ifndef __UARTSWTX__
#define __UARTSWTX__
#include "avrlibdefs.h"
/**
 * @brief Configures the required pin functions for the UART
 *
 */
void UARTSWTX_init();

/**
 * @brief Low level function to transmit a byte using the UART
 *
 * @param c byte to transmit
 * @param blocking no pupose here since this function always immediately
 * transmits
 * @return true transmition of byte was successful
 * @return false is never returned since this function always immediately
 * transmits
 */
bool UARTSWTX_TransmiteByte(uint8_t c, bool blocking);

#endif // __UARTSWTX__
