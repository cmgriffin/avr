#ifndef PID_H
#define PID_H

#include "fixedpt.h"

typedef struct
{
    Q7_8 setpoint;
    Q7_8 kp;
    Q7_8 ki;
    Q7_8 kd;
    Q7_8 output_max;
    Q15_16 output_sum;
    Q7_8 last_input;
} PID_t;

typedef struct
{
    Q7_8 setpoint;
    Q7_8 kp;
    Q7_8 ki;
    Q7_8 kd;
    Q7_8 output_max;
    Q7_8 starting_output;
    Q7_8 starting_input;
} PID_init_t;

void PID_init(PID_t *pid, PID_init_t *init);

Q7_8 PID_update(PID_t *pid, Q7_8 input);

void PID_update_coefficients(PID_t *pid, Q7_8 Kp, Q7_8 Ki, Q7_8 Kd);

void PID_update_setpoint(PID_t *pid, Q7_8 setpoint);

#endif /* PID_H */