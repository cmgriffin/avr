#ifndef PID_H
#define PID_H

typedef struct
{
    double setpoint;
    double kp;
    double ki;
    double kd;
    double output_max;
    double output_sum;
    double last_input;
} PID_t;

typedef struct
{
    double setpoint;
    double kp;
    double ki;
    double kd;
    double output_max;
    double starting_output;
    double starting_input;
} PID_init_t;

void PID_init(PID_t *pid, PID_init_t *init);

double PID_update(PID_t *pid, double input);

void PID_update_coefficients(PID_t *pid, double Kp, double Ki, double Kd);

void PID_update_setpoint(PID_t *pid, double setpoint);

#endif /* PID_H */