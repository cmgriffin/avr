#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

typedef struct
{
    double kp;
    double tau;
    double output;
    uint8_t deadtime;
    double *input_hist;
} MODEL_t;

typedef struct
{
    double kp;
    double tau;
    uint8_t deadtime;
    double initial_output;
    double initial_input;
} MODEL_init_t;

void MODEL_init(MODEL_t *model, MODEL_init_t *init);

double MODEL_update(MODEL_t *model, double input);

#endif /* MODEL_H */