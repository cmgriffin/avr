#include "pid.h"
#include "utils.h"

void PID_init(PID_t *pid, PID_init_t *init)
{
    pid->setpoint   = init->setpoint;
    pid->kp         = init->kp;
    pid->ki         = init->ki;
    pid->kd         = init->kd;
    pid->output_max = init->output_max;
    pid->output_sum = 0;
    pid->last_input = init->starting_input;
}

Q7_8 PID_update(PID_t *pid, Q7_8 input)
{
    Q7_8 error            = pid->setpoint - input;
    Q7_8 d_input          = input - pid->last_input;
    Q15_16 integrator_max = (Q15_16)pid->output_max << 16;

    pid->output_sum += (Q15_16)pid->ki * error;
    pid->output_sum = bound(pid->output_sum, integrator_max);

    Q7_8 p_weight   = bound(((Q15_16)pid->kp * error) >> 8, pid->output_max);
    Q7_8 i_weigth   = pid->output_sum >> 16;
    Q7_8 d_weight   = pid->kd * d_input;

    Q7_8 output     = bound(p_weight + i_weigth - d_weight, pid->output_max);

    pid->last_input = input;

    return output;
}

void PID_update_coefficients(PID_t *pid, Q7_8 Kp, Q7_8 Ki, Q7_8 Kd)
{
    pid->kp         = Kp;
    pid->output_sum = FIXEDPT_DIV(FIXEDPT_MULT(pid->output_sum, pid->ki), Ki);
    pid->ki         = Ki;
    pid->kd         = Kd;
}

void PID_update_setpoint(PID_t *pid, Q7_8 setpoint)
{
    pid->setpoint = setpoint;
}