#ifndef AUTOTUNE_H
#define AUTOTUNE_H

typedef struct
{
    int time;
    double value;
} AUTOTUNE_peak_t;

typedef struct
{
    double setpoint;
    double output_step;
    double Ku;
    double Pu;
    uint8_t num_peaks;
    uint8_t peak_number;
    uint8_t direction : 1;
    uint8_t state     : 1;
    AUTOTUNE_peak_t current_peak;
    AUTOTUNE_peak_t *peaks;
} AUTOTUNE_t;

typedef struct
{
    double setpoint;
    double output_step;
    double num_cycles;
    AUTOTUNE_peak_t *peak_arr;
} AUTOTUNE_init_t;

void AUTOTUNE_init(AUTOTUNE_t *atune, AUTOTUNE_init_t *init);

int AUTOTUNE_update(AUTOTUNE_t *atune, double input, double *pOutput, int time);

void AUTOTUNE_print_peaks(AUTOTUNE_t *atune);

double AUTOTUNE_get_Kp(AUTOTUNE_t *atune);
double AUTOTUNE_get_Ki(AUTOTUNE_t *atune);
double AUTOTUNE_get_Kd(AUTOTUNE_t *atune);

#endif /* AUTOTUNE_H */