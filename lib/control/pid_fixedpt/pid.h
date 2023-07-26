#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct
{
    int16_t setpoint;
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int16_t output_max;
    int32_t output_sum;
    int8_t sat;
    int16_t last_input;
} PID_t;

typedef struct
{
    int16_t setpoint;
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int16_t output_max;
    int16_t starting_output;
    int16_t starting_input;
} PID_init_t;

void PID_init(PID_t *pid, PID_init_t *init);

int16_t PID_update(PID_t *pid, int16_t input);

void PID_update_setpoint(PID_t *pid, int16_t setpoint);

void PID_reset(PID_t *pid);

#endif /* PID_H */