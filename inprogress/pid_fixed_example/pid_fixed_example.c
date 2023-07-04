#include "global.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

// #include "autotune.h"
#include "fixedpt.h"
#include "model.h"
#include "pid.h"
#include "utils.h"

MODEL_t model;
PID_t pid;
// AUTOTUNE_t atune;

int main(void)
{
    UART_init();
    print("Program Started\n");

    MODEL_init(&model, &(MODEL_init_t){.kp             = FIXEDPT_CONST(2),
                                       .tau            = FIXEDPT_CONST(10),
                                       .deadtime       = 5,
                                       .initial_output = FIXEDPT_CONST(0),
                                       .initial_input  = FIXEDPT_CONST(0)});

    PID_init(&pid, &(PID_init_t){.setpoint        = FIXEDPT_CONST(40),
                                 .kp              = FIXEDPT_CONST(0.433),
                                 .ki              = FIXEDPT_CONST(0.05),
                                 .kd              = FIXEDPT_CONST(0),
                                 .output_max      = FIXEDPT_CONST(10),
                                 .starting_output = FIXEDPT_CONST(0),
                                 .starting_input  = FIXEDPT_CONST(0)});

    // AUTOTUNE_peak_t peak_arr[10] = {};
    // AUTOTUNE_init(&atune, &(AUTOTUNE_init_t){.setpoint    = 1,
    //                                          .output_step = 1,
    //                                          .num_cycles  = 2,
    //                                          .peak_arr    = peak_arr});

    int loop       = 0;
    int16_t input  = FIXEDPT_CONST(1);
    int16_t output = FIXEDPT_CONST(0);
    int iinput, ioutput;

    // // run autotuning
    // for (loop = 0; loop < 200; loop++)
    // {

    //     output = MODEL_update(&model, input);
    //     print("%d,%d\n", input, output);
    //     // if (loop == 100)
    //     // {
    //     //     input = FIXEDPT_CONST(0.5);
    //     // }
    //     // if (!AUTOTUNE_update(&atune, output, &input, loop))
    //     // {
    //     //     print("loop finished\n");
    //     //     break;
    //     // }

    //     loop++;
    // }
    // AUTOTUNE_print_peaks(&atune);
    // int Ku, Pu, Kp, Ki, Kd;
    // Ku = (int)(1000 * atune.Ku);
    // Pu = (int)(1000 * atune.Pu);
    // print("Ku=%d, Pu=%d\n", Ku, Pu);

    // Kp = (int)(1000 * AUTOTUNE_get_Kp(&atune));
    // Ki = (int)(1000 * AUTOTUNE_get_Ki(&atune));
    // Kd = (int)(1000 * AUTOTUNE_get_Kd(&atune));

    // print("Kp=%d, Ki=%d, Kd=%d\n", Kp, Ki, Kd);

    // PID_update_coefficients(&pid, AUTOTUNE_get_Kp(&atune),
    //                         AUTOTUNE_get_Ki(&atune),
    //                         AUTOTUNE_get_Kd(&atune));

    print("\nrunning!\n");

    for (loop = 0; loop < 100; loop++)
    {
        print("%d,%d\n", input, output);
        output = MODEL_update(&model, input);
        input  = PID_update(&pid, output);
        if (loop == 49)
        {
            PID_update_setpoint(&pid, FIXEDPT_CONST(10));
        }
    }

    return 0;
}
