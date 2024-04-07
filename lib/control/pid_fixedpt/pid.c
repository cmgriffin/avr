#include "control/pid_fixedpt/pid.h"
#include "control/pid_fixedpt/fixedpoint_utils.h"
#include "utils.h"
#include <debug_minimal.h>
#include <stdlib.h>

/**
 * @brief
 *
 * @param sat
 * @param x
 * @param min
 * @param max
 * @return Q32
 */
static int32_t __satlimit(int8_t *sat, int32_t x, int32_t limit)
{
    if (sat == NULL)
    {
        int8_t dummy;
        sat = &dummy;
    }
    if (x <= -1 * limit)
    {
        *sat = -1;
        return -1 * limit;
    }
    else if (x >= limit)
    {
        *sat = 1;
        return limit;
    }
    else
    {
        *sat = 0;
        return x;
    }
}

void PID_init(PID_t *pid, PID_init_t *init)
{
    pid->setpoint   = init->setpoint;
    pid->kp         = init->kp;
    pid->ki         = init->ki;
    pid->kd         = init->kd;
    pid->deadband   = init->deadband;
    pid->output_max = init->output_max;
    pid->output_sum = 0;
    pid->sat        = 0;
    pid->last_input = init->starting_input;
    pid->last_output = 0;
}

int16_t PID_update(PID_t *pid, int16_t input)
{
    int16_t error   = pid->setpoint - input;
    int16_t d_input = input - pid->last_input;
    pid->last_input = input;

    if (pid->deadband*-1 <= error && error <= pid->deadband){
        return pid->last_output;
    }
    
    // handle the integrator
    if (!((pid->sat < 0 && error < 0) || (pid->sat > 0 && error > 0)))
    {
        pid->output_sum += ((int32_t)pid->ki) * error;
    }
    pid->output_sum = __satlimit(&(pid->sat), pid->output_sum,
                                 (int32_t)pid->output_max << SCALE_FACTOR);

    int32_t p_term  = __satlimit(NULL, (int32_t)pid->kp * error,
                                 (int32_t)pid->output_max << SCALE_FACTOR);
    int32_t d_term  = (int32_t)pid->kd * d_input;

    int16_t output  = (int16_t)__satlimit(NULL,
                                          (p_term >> SCALE_FACTOR) +
                                              (pid->output_sum >> SCALE_FACTOR) -
                                              (d_term >> SCALE_FACTOR),
                                          pid->output_max);

    pid->last_output = output;

    DEBUG_print("PID_state: output_sum=");
    DEBUG_printnum(pid->output_sum);
    DEBUG_print(" sat=");
    DEBUG_printnum(pid->sat);
    DEBUG_print(" error=");
    DEBUG_printnum(error);
    DEBUG_print(" input=");
    DEBUG_printnum(input);
    DEBUG_print(" p_term=");
    DEBUG_printnum(p_term);
    DEBUG_print(" d_term=");
    DEBUG_printnum(d_term);
    DEBUG_print(" output=");
    DEBUG_printnum(output);
    DEBUG_print("\n");

    return output;
}

void PID_update_setpoint(PID_t *pid, int16_t setpoint)
{
    pid->setpoint = setpoint;
}

void PID_update_coefficients(PID_t *pid, int16_t Kp, int16_t Ki, int16_t Kd)
{
    pid->kp         = Kp;
    pid->output_sum = pid->output_sum * pid->ki / Ki;
    pid->ki         = Ki;
    pid->kd         = Kd;
}

void PID_reset(PID_t *pid)
{
    pid->output_sum = 0;
    pid->sat        = 0;
    pid->last_input = 0;
}