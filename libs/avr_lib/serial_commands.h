#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H
/*
 * Implements serial callback commands
 * When the handleSerialCommands function is called it is passed a string pointer having the command to be parsed.
 * The list of command strings is looped through and the matching command is found.
 * The corresponding callback function is then called passing the remaining arguments.
 * The responsibility is given to the callback function to parse out and handle the arguments
 */

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdbool.h>
#include <stdint.h>
    // container for args passed to call back function

    typedef struct callback_args
    {
        char *cmd_args;
        bool query;
        void *var;
    } callback_args_t;

    typedef int8_t (*callback_t)(callback_args_t *);
    typedef void (*result_callback_t)(void);

    // contains the token and callback function of a single command
    typedef const struct cmd
    {
        char *cmd_str;       // the command token
        callback_t callback; // callback function
        void *var;           // the state variable associated with the command
    } cmd_t;

    // contains all required parameters for the command parser
    typedef const struct serial_command
    {
        char *delimiter;
        uint8_t n_cmds;
        cmd_t *cmds;
        result_callback_t ok_cmd;
        result_callback_t error_cmd;
    } serial_command_t;

    // the function that actually parses the command string and calls the
    // requested callback function
    void handleSerialCommands(const serial_command_t *sCmd, char *cmd);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_COMMANDS_H