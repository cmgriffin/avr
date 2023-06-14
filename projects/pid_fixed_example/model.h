#ifndef MODEL_H
#define MODEL_H

#include "fixedpt.h"
#include <avrlibdefs.h>

#define MODEL_PARAM_SHIFT 4
typedef struct
{
    Q7_8 kp;
    Q7_8 tau;
    Q7_8 output;
    uint8_t deadtime;
    Q7_8 *input_hist;
} MODEL_t;

typedef struct
{
    Q7_8 kp;
    Q7_8 tau;
    int deadtime;
    Q7_8 initial_output;
    Q7_8 initial_input;
} MODEL_init_t;

void MODEL_init(MODEL_t *model, MODEL_init_t *init);

int16_t MODEL_update(MODEL_t *model, int16_t input);

#endif /* MODEL_H */