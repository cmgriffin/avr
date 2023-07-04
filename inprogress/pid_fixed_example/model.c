#include <stdlib.h>

#include "global.h"
#include "model.h"

void MODEL_init(MODEL_t *model, MODEL_init_t *init)
{
    model->kp       = init->kp;
    model->tau      = init->tau;
    model->output   = init->initial_output;
    model->deadtime = init->deadtime;

    int16_t *input_hist =
        calloc(init->deadtime + 1, sizeof(init->initial_input));
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

int16_t MODEL_update(MODEL_t *model, int16_t input)
{

    for (int i = 0; i < (model->deadtime); i++)
    {
        model->input_hist[i] = model->input_hist[i + 1];
    }

    model->input_hist[model->deadtime] = input;

    // int16_t new_output = ((model->kp / model->tau) * model->input_hist[0]) +
    //                      (model->output * 1 - 1 / model->tau);

    Q7_8 new_output = FIXEDPT_MULT(model->kp, model->input_hist[0]);
    new_output -= model->output;
    new_output += FIXEDPT_MULT(model->tau, model->output);
    new_output    = FIXEDPT_DIV(new_output, model->tau);

    model->output = new_output;
    return new_output;
}