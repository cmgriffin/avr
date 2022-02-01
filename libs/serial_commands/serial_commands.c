#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "serial_commands.h"

#ifdef DEBUG
#define debug_printStr(p_str, str)        \
    {                                     \
        UART_printStr_p(PSTR("DEBUG: ")); \
        UART_printStr_p(PSTR(p_str));     \
        UART_printStr(str);               \
        UART_printStr_p(PSTR("\n"));      \
    }
#else
#define debug_printStr(p_str, str)
#endif

void handleSerialCommands(const serial_command_t *sCmd, char *cmd)
{
    char *token;
    uint8_t foundCmd = 0;

    debug_printStr("command string received: ", cmd);
    token = strtok_P(cmd, sCmd->delimiter); // get the command token
    //token = strtok(cmd, " "); // get the command token
    debug_printStr("command key received: ", token);

    for (uint8_t i = 0; i < pgm_read_byte(sCmd->n_cmds); i++)
    {
        if (strcmp_P(token, sCmd->cmds[i].cmd_str) == 0)
        {
            //debug_printStr("matching command key found: ", current_cmd);
            foundCmd = 1;
            //(*callbacks[i])(strtok(NULL, "")); // pass all the args to the callback function
            ((callback_t)pgm_read_word(sCmd->cmds[i].callback))(strtok(NULL, ""));
            break;
        }
    }
    if (!foundCmd) // executed if the command didn't match any of the options
    {
        UART_printStr_p(PSTR("Command wasn't found\n"));
    }
}
