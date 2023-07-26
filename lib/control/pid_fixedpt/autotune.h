#ifndef AUTOTUNE_H
#define AUTOTUNE_H

#include <stdint.h>

typedef struct
{
    int16_t kp;
    int16_t ki;
    int16_t kd;
} AUTOTUNE_coefficients_t;

#define AUTOTUNE_COEF_CLASSIC_PID                                 \
    {                                                             \
        .kp = FIXEDPT_CONST(0.6), .ki = FIXEDPT_CONST(1.2), .kd = FIXEDPT_CONST(0.075) \
    }
#define AUTOTUNE_COEF_PI                                        \
    {                                                           \
        .kp = FIXEDPT_CONST(0.45), .ki = FIXEDPT_CONST(0.54), .kd = FIXEDPT_CONST(0) \
    }
#define AUTOTUNE_COEF_PID_NO_VERSHOOT                             \
    {                                                             \
        .kp = FIXEDPT_CONST(0.2), .ki = FIXEDPT_CONST(0.4), .kd = FIXEDPT_CONST(0.066) \
    }

typedef struct
{
    uint16_t time;
    int16_t value;
} AUTOTUNE_peak_t;

typedef struct
{
    int16_t setpoint;
    int16_t output_step;
    int16_t Ku;
    int16_t Pu;
    uint8_t num_peaks;
    uint8_t peak_number;
    uint8_t direction   : 1;
    uint8_t state       : 1;
    uint8_t process_dir : 1;
    AUTOTUNE_peak_t current_peak;
    AUTOTUNE_peak_t *peaks;
} AUTOTUNE_t;

typedef struct
{
    int16_t setpoint;
    int16_t output_step;
    uint8_t num_cycles;
    AUTOTUNE_peak_t *peak_arr;
} AUTOTUNE_init_t;

void AUTOTUNE_init(AUTOTUNE_t *atune, AUTOTUNE_init_t *init);

uint8_t AUTOTUNE_update(AUTOTUNE_t *atune, int16_t input, int16_t *pOutput,
                        uint16_t time);

void AUTOTUNE_print_peaks(AUTOTUNE_t *atune);

int16_t AUTOTUNE_get_Kp(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs);
int16_t AUTOTUNE_get_Ki(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs);
int16_t AUTOTUNE_get_Kd(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs);

#endif /* AUTOTUNE_H */