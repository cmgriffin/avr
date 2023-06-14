#ifndef CONTROL_SCMD_H
#define CONTROL_SCMD_H
/*
 * Implements serial callback commands
 * When the handleSerialCommands function is called it is passed a string
 * pointer having the command to be parsed. The list of command strings is
 * looped through and the matching command is found. The corresponding callback
 * function is then called passing the remaining arguments. The responsibility
 * is given to the callback function to parse out and handle the arguments
 */

#include <stdbool.h>
#include <stdint.h>

// container for args passed to call back function
typedef struct
{
    char *cmd_args; // actual arguments supplied in string form
    bool query;     // indicates if the call is for a query or write
    void *var;      // variable associated with the command
} SCMD_callback_args_t;

// typedef for the command callback function(s)
typedef int8_t (*SCMD_callback_t)(SCMD_callback_args_t *);

// typedef for the result callback functions
typedef void (*SCMD_result_callback_t)(void);

// contains the token and callback function of a single command
typedef struct
{
    const char *cmd_str;      // the command token
    SCMD_callback_t callback; // callback function
    void *var;                // the state variable associated with the command
} SCMD_cmd_t;

// contains all required parameters for the command parser
typedef struct
{
    char *delimiter;               // delimiter to use when parsing the command
    uint8_t n_cmds;                // number of commands
    const SCMD_cmd_t *cmds;        // list of the commands in flash
    SCMD_result_callback_t ok_cmd; // function to call when a comnmand succeeds
    SCMD_result_callback_t error_cmd; // function to call when a command fails
} SCMD_t;

/**
 * @brief Processes the provided command string and calls the requested
 * callbacks
 *
 * @param sCmd pointer to an instance of SCMD_t
 * @param msg command string to be processed
 */
void SCMD_processCmd(const SCMD_t *sCmd, char *msg);

#endif // CONTROL_SCMD_H