#include "stream.h"
#include "global.h"
#include <avr/pgmspace.h>
#include <stdbool.h>

const uint16_t PROGMEM powers_of_ten[] = {0, 1, 10, 100, 1000, 10000};

int STREAM_putChar(char c, stream_t *io, FILE *stream)
{
    if (c == '\n')
        STREAM_putChar('\r', io, stream);

    io->tx_func((uint8_t)c, true); // transmit c and block if necessary
    return 0;
}

int STREAM_getChar(char *s, uint8_t n, char **rxptr, stream_t *io, FILE *stream)
{
    char *cptr; // ptr to current position in the buffer when loading
    uint8_t c;

    if (!*rxptr)
    {
        // nothing loaded up for the getChar yet

        for (cptr = s; !*rxptr;)
        {
            io->rx_func(&c, true); // receivce into c and block if necessary
            switch (c)
            {
            case '\r':
                break;
                // c = '\n';
            case '\n':
                // line has been loaded set the read pointer
                *cptr = c;
                STREAM_putChar(c, io, stream);
                *rxptr = s;
                break;
            case '\b': // backspace
                if (cptr > s)
                {
                    // only backspace if buf is not empty
                    STREAM_putChar('\b', io, stream); // move back one
                    STREAM_putChar(' ', io, stream);  // clear the char
                    STREAM_putChar('\b', io, stream); // move back
                    cptr--;
                }
                break;
            default:
                if (!(cptr == s + n - 1))
                {
                    *cptr++ = c;
                    STREAM_putChar(c, io, stream);
                }
            }
        }
    }
    // c = **rxptr; // get the char in the array
    // (*rxptr)++;  // increment to next position in the array
    c = *(*rxptr)++; // equivalent to above
    if (c == '\n')
    {
        *rxptr = 0;
    }
    return c;
}

char *STREAM_readLine(char *s, uint8_t n, char **rxptr, stream_t *io)
{
    uint8_t c;

    // fetch the next byte, abort loop if nothing is available
    // increment the pointer to the buffer position
    for (; (io->rx_func(&c, false));)
    {
        switch (c)
        {
        case '\r':
            break;
            // c = '\n';
        case '\n':
            // line has been loaded set the read pointer
            **rxptr = '\0';                // add null to end the string
            io->tx_func((uint8_t)c, true); // transmit c and block if necessary
            *rxptr = s;                    // reset the pointer
            return s;                      // let the calling function know the buffer is ready

        case '\b': // backspace
            if (*rxptr > s)
            {
                // only backspace if buf is not empty
                io->tx_func((uint8_t)'\b', true); // move back one
                io->tx_func((uint8_t)' ', true);  // clear the char
                io->tx_func((uint8_t)'\b', true); // move back
                (*rxptr)--;
            }
            break;
        default:
            if (!(*rxptr == s + n - 1))
            {
                *(*rxptr)++ = c;
                io->tx_func((uint8_t)c, true); // move back
            }
        }
    }
    return NULL;
}

void STREAM_printStr(const char *s, stream_t *io)
{
    while (*s)
    {
        if (*s == '\n')
        {
            io->tx_func('\r', true);
        }
        io->tx_func(*s++, true);
    }
}

void STREAM_printStr_p(const char *s, stream_t *io)
{
    char c;

    for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
    {
        if (c == '\n')
            io->tx_func('\r', true);
        io->tx_func(c, true);
    }
}

void STREAM_print_u16(uint16_t v, uint8_t fp, bool rj, bool neg_sign, stream_t *io)
{
    /*
    * a total of 5 digits implemented due to 65,536 being max for unsigned integer

    */
    uint8_t first_digit = 1;
    if (rj && !neg_sign)
    {
        io->tx_func(' ', true);
    }
    for (uint8_t pos = 5; pos > 0; pos--)
    {

        uint16_t n = (v / pgm_read_word(&powers_of_ten[pos])) % 10;

        if (pos == fp)
        { // separate conditional since the decimal point gets printed right before the selected character
            if (first_digit)
            {
                if (neg_sign)
                {
                    io->tx_func('-', true);
                }
                io->tx_func('0', true);
                first_digit = 0;
            }
            io->tx_func('.', true);
        }
        if ((n != 0) && (first_digit))
        {
            if (neg_sign)
            {
                io->tx_func('-', true);
            }
            io->tx_func('0' + n, true);
            first_digit = 0;
        }
        else if (rj && first_digit && !((pos - 1) == fp))
        {
            io->tx_func(' ', true);
        }
        else if (!first_digit)
        {
            io->tx_func('0' + n, true);
        }
    }
}

void STREAM_print_i16(int16_t v, uint8_t fp, bool rj, stream_t *io)
{
    if (v < 0)
    {
        v *= -1;
        STREAM_print_u16(v, fp, rj, true, io);
    }
    else
    {
        STREAM_print_u16(v, fp, rj, false, io);
    }
}