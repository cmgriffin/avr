#if __has_include("uartswtx_conf.h")
#include "uartswtx.h"
#include "global.h"
#include "gpio.h"
#include "uartswtx_conf.h"
#include <util/delay.h>

// bit delay with adjustment based on measurements
// this is computed at compile time
#define BIT_DELAY() _delay_us(1E6 / UARTSWTX_BAUD - 0.56)

void UARTSWTX_init()
{
    GPIO_setValueHigh(&(GPIO_TypeDef)UARTSWTX_TX_PIN);
    GPIO_setOutput(&(GPIO_TypeDef)UARTSWTX_TX_PIN);
}

bool UARTSWTX_TransmitByte(uint8_t c, bool blocking)
{
    // send the start bit
    GPIO_setValueLow(&(GPIO_TypeDef)UARTSWTX_TX_PIN);
    BIT_DELAY();
    for (uint8_t i = 1; i; i <<= 1)
    {
        GPIO_setValueLogical(&(GPIO_TypeDef)UARTSWTX_TX_PIN, (i & c));
        BIT_DELAY();
    }
    // send the stop bit
    GPIO_setValueHigh(&(GPIO_TypeDef)UARTSWTX_TX_PIN);
    BIT_DELAY();
    return true;
}

#endif