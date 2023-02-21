#include "control/scmd.h"
#include "debug.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

bool checkForMatch(const char *token, const char *checkCmd,
                   SCMD_callback_args_t *cb_args)
{
    size_t checkCmdLength = strlen_P(checkCmd);
    if (!strncasecmp_P(token, checkCmd, checkCmdLength))
    {
        switch (token[checkCmdLength])
        {
        case '\0':
            // end of token string: a write command
            // Serial.println(F("found a write command"));
            break;
        case '?':
            // ? after the prefix indicates a query
            if (token[checkCmdLength + 1])
                // addional chars after expected end
                return false;
            cb_args->query = true;

            break;
        default:
            // some other character: not a real match
            // Serial.println(F("not a matching command after prefix"));
            return false;
        }
        return true;
    }
    return false;
}

void SCMD_processCmd(const SCMD_t *sCmd, char *msg)
{
    char *token;
    uint8_t foundCmd = 0;

    // get the pointer to the list of commands
    const SCMD_cmd_t *cmds = (const SCMD_cmd_t *)pgm_read_word(&sCmd->cmds);

    _DEBUG("command string received: %s", msg);
    token = strtok_P(
        msg, (PGM_P)pgm_read_word(&(sCmd->delimiter))); // get the command token
    _DEBUG("command key received: %s", token);

    for (uint8_t i = 0; i < pgm_read_byte(&(sCmd->n_cmds)); i++)
    {
        SCMD_callback_args_t cb_args = {
            .cmd_args = NULL, .query = false, .var = NULL};
        if (checkForMatch(token, (PGM_P)pgm_read_word(&(cmds[i].cmd_str)),
                          &cb_args))
        {
            foundCmd         = 1;
            cb_args.cmd_args = strtok_P(NULL, "");
            cb_args.var      = (void *)pgm_read_ptr(&(cmds[i].var));
            int8_t result =
                ((SCMD_callback_t)pgm_read_word(&(cmds[i].callback)))(&cb_args);
            if (result == 0)
                ((SCMD_result_callback_t)pgm_read_word(&(sCmd->ok_cmd)))();
            else
                ((SCMD_result_callback_t)pgm_read_word(&(sCmd->error_cmd)))();
            break;
        }
    }
    if (!foundCmd) // executed if the command didn't match any of the options
    {
        ((SCMD_result_callback_t)pgm_read_word(&(sCmd->error_cmd)))();
    }
}
