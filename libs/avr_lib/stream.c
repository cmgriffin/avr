#include "stream.h"
#include "global.h"

int STREAM_putChar(char c, stream_t *io, FILE *stream)
{
    if (c == '\n')
        STREAM_putChar('\r', io, stream);

    io->tx_func((uint8_t)c);
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
            c = UART_ReceiveByte();
            switch (c)
            {
            case '\r':
                break;
                // c = '\n';
            case '\n':
                // line has been loaded set the read pointer
                *cptr = c;
                UART_putChar(c, stream);
                *rxptr = s;
                break;
            case '\b': // backspace
                if (cptr > s)
                {
                    // only backspace if buf is not empty
                    UART_putChar('\b', stream); // move back one
                    UART_putChar(' ', stream);  // clear the char
                    UART_putChar('\b', stream); // move back
                    cptr--;
                }
                break;
            default:
                if (!(cptr == s + UART_GETCHAR_BUFFER_SIZE - 1))
                {
                    *cptr++ = c;
                    UART_putChar(c, stream);
                }
            }
        }
    }
    c = **rxptr++;
    if (c == '\n')
    {
        *rxptr = 0;
    }
    return c;
}
