#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "autotune.h"
#include "utils.h"

#define INCREASING 1
#define DECREASING 0

#define RUNNING    1
#define STOPPED    0

// Private Functions

void __save_peak(AUTOTUNE_t *atune){
    if (atune->peaks != NULL){
        atune->peaks[atune->peak_number] = (AUTOTUNE_peak_t){
            atune->current_peak.time, atune->current_peak.value};
    }
}

double __get_Ku(AUTOTUNE_t *atune)
{
    AUTOTUNE_peak_t *p;
    double maxima_sum = 0.0;
    double minima_sum = 0.0;
    for (int i = 1; i < atune->num_peaks; i++)
    {
        p = &(atune->peaks[i]);
        if (p->value > atune->setpoint)
        {
            maxima_sum += p->value;
        }
        else
        {
            minima_sum += p->value;
        }
    }
    double amplitude = maxima_sum / (atune->num_peaks / 2) -
                       minima_sum / (atune->num_peaks / 2 - 1);
    return 4.0 * atune->output_step / (amplitude * 3.14) / 2;
}

double __get_Pu(AUTOTUNE_t *atune)
{
    AUTOTUNE_peak_t *p;
    int pu_sum        = 0;
    int previous_peak = -1;
    for (int i = 1; i < atune->num_peaks; i++)
    {
        p = &(atune->peaks[i]);
        if (p->value > atune->setpoint)
        {
            if (previous_peak == -1)
            {
                previous_peak = p->time;
            }
            else
            {
                pu_sum += p->time - previous_peak;
                previous_peak = p->time;
            }
        }
    }
    return pu_sum / (atune->num_peaks / 2 - 1);
}

void __finish(AUTOTUNE_t *atune)
{
    atune->Ku = __get_Ku(atune);
    atune->Pu = __get_Pu(atune);
    print("__finish called\n");
}


// API functions

void AUTOTUNE_init(AUTOTUNE_t *atune, AUTOTUNE_init_t *init)
{
    atune->setpoint           = init->setpoint;
    atune->output_step        = init->output_step;
    atune->num_peaks          = (init->num_cycles + 1) * 2;
    atune->direction          = INCREASING;
    atune->peak_number        = 0;
    atune->current_peak.time  = 0;
    atune->current_peak.value = 0.0;
    atune->state              = STOPPED;
    atune->peaks              = init->peak_arr;
}

int AUTOTUNE_update(AUTOTUNE_t *atune, double input, double *pOutput, int time)
{

    if (!atune->state)
    {
        // initialize things
        atune->state = RUNNING;
        *pOutput     = atune->output_step;
    }

    if (atune->direction == DECREASING)
    {

        if (atune->current_peak.value < input)
        {
            // update the current min if the input is less
            atune->current_peak = (AUTOTUNE_peak_t){time, input};
        }

        if (input < atune->setpoint)
        {
            // toggle the output if the setpoint has been passed
            *pOutput += atune->output_step;
            atune->direction                 = INCREASING;

            // save the current minimum in the history
            __save_peak(atune);
            atune->peak_number++;
            // register the starting value for the next maximum
            atune->current_peak = (AUTOTUNE_peak_t){time, input};
        }
    }
    else if (atune->direction == INCREASING)
    {

        if (atune->current_peak.value > input)
        {
            atune->current_peak = (AUTOTUNE_peak_t){time, input};
        }

        if (input > atune->setpoint)
        {
            *pOutput -= atune->output_step;
            atune->direction                 = DECREASING;
            __save_peak(atune);
            atune->peak_number++;
            atune->current_peak = (AUTOTUNE_peak_t){time, input};
        }
    }

    if (atune->num_peaks == atune->peak_number - 1)
    {
        __finish(atune);
        print("AUTOTUNE_update end\n");
        return STOPPED;
    }

    return RUNNING;
}



void AUTOTUNE_print_peaks(AUTOTUNE_t *atune)
{
    AUTOTUNE_peak_t *p;
    for (int i = 0; i < atune->num_peaks; i++)
    {
        p = &(atune->peaks[i]);
        print("peak #%d: %d at %d\n", i, (int)(p->value * 1000), p->time);
    }
}






double AUTOTUNE_get_Kp(AUTOTUNE_t *atune) { return 0.6 * atune->Ku; }

double AUTOTUNE_get_Ki(AUTOTUNE_t *atune)
{
    return 1.2 * atune->Ku / atune->Pu;
}

double AUTOTUNE_get_Kd(AUTOTUNE_t *atune)
{
    return 0.075 * atune->Ku * atune->Pu;
}