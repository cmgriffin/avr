#include "global.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

#include "autotune.h"
#include "model.h"
#include "pid.h"
#include "utils.h"

MODEL_t model;
PID_t pid;
AUTOTUNE_t atune;

int main(void)
{
    UART_init();
    print("Program Started\n");

    MODEL_init(&model, &(MODEL_init_t){.kp             = 2,
                                       .tau            = 10,
                                       .deadtime       = 5,
                                       .initial_output = 0,
                                       .initial_input  = 0});

    PID_init(&pid, &(PID_init_t){.setpoint        = 10,
                                 .kp              = 3.06,
                                 .ki              = 0.6,
                                 .kd              = 3.825,
                                 .output_max      = 10,
                                 .starting_output = 0,
                                 .starting_input  = 0});

    AUTOTUNE_peak_t peak_arr[10] = {};
    AUTOTUNE_init(&atune, &(AUTOTUNE_init_t){.setpoint    = 1,
                                             .output_step = 1,
                                             .num_cycles  = 2,
                                             .peak_arr    = peak_arr});

    int loop      = 0;
    double input  = 0;
    double output = 0;
    int iinput, ioutput;

    // run autotuning
    for (;;)
    {

        output = MODEL_update(&model, input);

        if (!AUTOTUNE_update(&atune, output, &input, loop))
        {
            print("loop finished\n");
            break;
        }

        loop++;
    }
    AUTOTUNE_print_peaks(&atune);
    int Ku, Pu, Kp, Ki, Kd;
    Ku = (int)(1000 * atune.Ku);
    Pu = (int)(1000 * atune.Pu);
    print("Ku=%d, Pu=%d\n", Ku, Pu);

    Kp = (int)(1000 * AUTOTUNE_get_Kp(&atune));
    Ki = (int)(1000 * AUTOTUNE_get_Ki(&atune));
    Kd = (int)(1000 * AUTOTUNE_get_Kd(&atune));

    print("Kp=%d, Ki=%d, Kd=%d\n", Kp, Ki, Kd);

    PID_update_coefficients(&pid, AUTOTUNE_get_Kp(&atune),
                            AUTOTUNE_get_Ki(&atune), AUTOTUNE_get_Kd(&atune));

    print("\nrunning!\n");

    for (loop = 0; loop < 100; loop++)
    {
        iinput  = (int)(1000 * input);
        ioutput = (int)(1000 * output);
        print("%d,%d\n", iinput, ioutput);
        output = MODEL_update(&model, input);
        input  = PID_update(&pid, output);
        if (loop == 49)
        {
            PID_update_setpoint(&pid, 0);
        }
    }

    return 0;
}
