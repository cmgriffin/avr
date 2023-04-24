#include "pid.h"
#include "utils.h"

void PID_init(PID_t *pid, PID_init_t *init)
{
    pid->setpoint   = init->setpoint;
    pid->kp         = init->kp;
    pid->ki         = init->ki;
    pid->kd         = init->kd;
    pid->output_max = init->output_max;
    pid->output_sum = bound(init->starting_output, init->output_max);
    pid->last_input = init->starting_input;
}

double PID_update(PID_t *pid, double input)
{
    double error   = pid->setpoint - input;
    double d_input = input - pid->last_input;

    pid->output_sum += pid->ki * error;
    pid->output_sum = bound(pid->output_sum, pid->output_max);

    double p_weight = pid->kp * error;
    double i_weigth = pid->output_sum;
    double d_weight = pid->kp * d_input;

    double output   = p_weight + i_weigth - d_weight;

    pid->last_input = input;

    return output;
}

void PID_update_coefficients(PID_t *pid, double Kp, double Ki, double Kd){
    pid->kp = Kp;
    pid->output_sum = pid->output_sum * pid->ki / Ki;
    pid->ki = Ki;
    pid->kd = Kd;
}

void PID_update_setpoint(PID_t *pid, double setpoint){
    pid->setpoint = setpoint;
}