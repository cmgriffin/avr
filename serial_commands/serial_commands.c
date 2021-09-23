#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "serial_commands.h"

#ifdef DEBUG
    #define debug_printStr(p_str, str) {\
        UART_printStr_p(PSTR("DEBUG: "));\
        UART_printStr_p(PSTR(p_str));\
        UART_printStr(str);\
        UART_printStr_p(PSTR("\n"));\
    }
#else
    #define debug_printStr(p_str, str) 
#endif

const char* cmds[] = {
    CMD_0,
    CMD_1,
    CMD_2,
    CMD_3,
};

void (*callbacks[4])() = {
    callback0,
    callback1,
    callback2,
    callback3
};

void handleSerialCommands(char *cmd)
{
    char *token;
    uint8_t foundCmd = 0;

    debug_printStr("command string received: ", cmd);

    token = strtok(cmd, DELIM); // get the command token
    
    debug_printStr("command key received: ", token);
    
    for(uint8_t i=0; i<=3; i++)
    {
        if(strcmp(cmds[i], token)==0)
        {
            debug_printStr("matching command key found: ", cmds[i]);
            foundCmd = 1;
            (*callbacks[i])(strtok(NULL, "")); // pass all the args to the callback function
            break;
        }
    }
    if (!foundCmd) // executed if the command didn't match any of the options
    {
        UART_printStr_p(PSTR("Command wasn't found\n"));
    }
}

void callback0(char *args) 
{
    UART_printStr_p(PSTR("callback0 executed\n"));
    parse_print_args(args);
}
void callback1(char *args) 
{
    UART_printStr_p(PSTR("callback1 executed\n"));
    parse_print_args(args);
}
void callback2(char *args) 
{
    UART_printStr_p(PSTR("callback2 executed\n"));
    parse_print_args(args);
}
void callback3(char *args) 
{
    UART_printStr_p(PSTR("callback3 executed\n"));
    parse_print_args(args);
}

void parse_print_args(char *args)
{
    uint8_t i = 0;
    char s[3];
    char *arg;
    arg = strtok(args, DELIM);
    while(arg != NULL)
    {
        UART_printStr_p(PSTR("ARG #"));
        UART_printStr(itoa(i, s, 10));
        UART_printStr_p(PSTR(": "));
        UART_printStr(arg);
        UART_printStr_p(PSTR("\n"));
        i++;
        arg = strtok(NULL, DELIM);
    }
}