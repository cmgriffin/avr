#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

typedef struct
{
    int16_t kp;
    int16_t tau;
    int16_t output;
    uint8_t deadtime;
    int16_t *input_hist;
} MODEL_t;

typedef struct
{
    int16_t kp;
    int16_t tau;
    uint8_t deadtime;
    int16_t initial_output;
    int16_t initial_input;
} MODEL_init_t;

void MODEL_init(MODEL_t *model, MODEL_init_t *init);

double MODEL_update(MODEL_t *model, int16_t input);

#endif /* MODEL_H */