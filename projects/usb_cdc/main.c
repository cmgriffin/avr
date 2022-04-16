
#include "global.h"

#include <avrlibdefs.h>
#include <stream.h>
#include <util/delay.h>

#include "m_usb.h"

static inline bool tx_char(uint8_t c, bool blocking)
{
    return !(m_usb_tx_char(c) == -1);
}

static inline bool rx_char(uint8_t *c, bool blocking)
{
    return !((*c = m_usb_rx_char()) == -1);
}

int main(void)
{
    stream_t usb_stream = STREAM_CREATE(tx_char, rx_char);

    usb_init();
    while (!m_usb_isconnected())
    {
    }
    for (;;)
    {
        STREAM_printStr("Hello World!!!\n", &usb_stream);
        //_delay_ms(1000);
    }
    return 0;
}
