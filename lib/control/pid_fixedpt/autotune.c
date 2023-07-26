#include <avr/pgmspace.h>
#include <debug_minimal.h>
// #include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <utils.h>

#include <control/pid_fixedpt/autotune.h>
#include <control/pid_fixedpt/fixedpoint_utils.h>
#include <control/pid_fixedpt/pid.h>

#define INCREASING 1
#define DECREASING 0

#define RUNNING    1
#define STOPPED    0

#define HEATING    0
#define COOLING    1

// Private Functions

void __save_peak(AUTOTUNE_t *atune)
{
    if (atune->peaks != NULL)
    {
        atune->peaks[atune->peak_number] = (AUTOTUNE_peak_t){
            atune->current_peak.time, atune->current_peak.value};
    }
}

// BUG: verflow issue here, output scaling is way larger than it needs to be
// then multiplying by 4 overflows
int16_t __get_Ku(AUTOTUNE_t *atune)
{
    AUTOTUNE_peak_t *p;
    int32_t maxima_sum = 0;
    int32_t minima_sum = 0;
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

    int16_t amplitude = (int16_t)(maxima_sum / (atune->num_peaks >> 1) -
                                  minima_sum / ((atune->num_peaks >> 1) - 1));
    // println("calculated amplitude %f", TO_FLOAT(amplitude));
    return FIXEDPT_DIV(FIXEDPT_MULT(FIXEDPT_CONST(4.0), atune->output_step),
                       FIXEDPT_MULT(amplitude, FIXEDPT_CONST(3.14 * 2)));
}

int16_t __get_Pu(AUTOTUNE_t *atune)
{
    AUTOTUNE_peak_t *p;
    uint32_t pu_sum       = 0;
    int16_t previous_peak = -1;
    for (uint8_t i = 1; i < atune->num_peaks; i++)
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
    // println("pu_sum = %u", pu_sum);
    return (int16_t)(pu_sum / ((atune->num_peaks >> 1) - 1));
}

void __finish(AUTOTUNE_t *atune)
{
    atune->Ku = __get_Ku(atune);
    atune->Pu = __get_Pu(atune);
    DEBUG_println("__finish called\n");
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
    atune->current_peak.value = 0;
    atune->state              = STOPPED;
    atune->peaks              = init->peak_arr;
    atune->process_dir        = init->output_step < 0 ? COOLING : HEATING;
}

uint8_t AUTOTUNE_update(AUTOTUNE_t *atune, int16_t input, int16_t *pOutput,
                        uint16_t time)
{

    if (!atune->state)
    {
        // initialize things
        atune->state     = RUNNING;
        atune->direction = input < atune->setpoint ? INCREASING : DECREASING;
        if (atune->direction == DECREASING && atune->process_dir == COOLING)
        {
            *pOutput = atune->output_step;
        }
        else if (atune->direction == INCREASING &&
                 atune->process_dir == HEATING)
        {
            *pOutput = atune->output_step;
        }
        else
        {
            *pOutput = 0;
        }
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
            if (atune->process_dir == COOLING)
            {
                *pOutput -= atune->output_step;
            }
            else
            {
                *pOutput += atune->output_step;
            }
            atune->direction = INCREASING;

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
            if (atune->process_dir == COOLING)
            {
                *pOutput += atune->output_step;
            }
            else
            {
                *pOutput -= atune->output_step;
            }
            atune->direction = DECREASING;
            __save_peak(atune);
            atune->peak_number++;
            atune->current_peak = (AUTOTUNE_peak_t){time, input};
        }
    }

    if (atune->num_peaks == atune->peak_number - 1)
    {
        __finish(atune);
        DEBUG_println("AUTOTUNE_update end\n");
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
        // print("peak #%d: %f at %d\n", i, TO_FLOAT(p->value), p->time);
    }
}

int16_t AUTOTUNE_get_Kp(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs)
{
    return FIXEDPT_MULT(coeffs->kp, atune->Ku);
}

int16_t AUTOTUNE_get_Ki(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs)
{
    return (FIXEDPT_MULT(coeffs->ki, atune->Ku) + (atune->Pu >> 1)) / atune->Pu;
}

int16_t AUTOTUNE_get_Kd(AUTOTUNE_t *atune, AUTOTUNE_coefficients_t *coeffs)
{
    return (FIXEDPT_MULT(coeffs->kd, atune->Ku) * atune->Pu);
}