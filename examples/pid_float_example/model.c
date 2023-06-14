#include <stdlib.h>

#include "model.h"

void MODEL_init(MODEL_t *model, MODEL_init_t *init)
{
    model->kp          = init->kp;
    model->tau         = init->tau;
    model->output      = init->initial_output;
    model->deadtime    = init->deadtime;

    double *input_hist = calloc(init->deadtime + 1, sizeof(double));
    if (input_hist == NULL)
    {
        return;
    }

    for (int i = 0; i < (init->deadtime + 1); i++)
    {
        input_hist[i] = init->initial_input;
    }
    model->input_hist = input_hist;
}

double MODEL_update(MODEL_t *model, double input)
{

    for (int i = 0; i < (model->deadtime); i++)
    {
        model->input_hist[i] = model->input_hist[i + 1];
    }

    model->input_hist[model->deadtime] = input;

    double new_output = ((model->kp / model->tau) * model->input_hist[0] +
                         model->output * (1 - 1 / model->tau));
    model->output     = new_output;
    return new_output;
}