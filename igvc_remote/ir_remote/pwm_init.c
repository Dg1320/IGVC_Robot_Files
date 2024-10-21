/*
 * pwminit.c
 *
 *  Created on: Mar 13, 2024
 *      Author: clutch13
 */

#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "pwm_init.h"
#include "uart0.h"




#define PIN_PC4 0b00010000
#define PIN_PC5 0b00100000


// MUST BE IMPLEMETED IN REGULAR TIMER INTERRUPTS AND THEN JUST CHANGE THE TIME REQUIREMENTS INSTEAD////!!!!!!!!!


void pwm_init()
{
    //---------Enable clocks
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1 | SYSCTL_RCGCPWM_R0;       // turn on pwm clock
    SYSCTL_RCGCGPIO_R |=  SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R5;  // gpio c/f clocks on

    _delay_cycles(3);

    //---------setup ports

    // Digital Enable Ports
    GPIO_PORTC_DEN_R |= PIN_PC4 | PIN_PC5;


    // Select Alternate Functions
    GPIO_PORTC_AFSEL_R |= PIN_PC4 | PIN_PC5;


    // Clear whatever value was in the original position
    GPIO_PORTC_PCTL_R &= ~(GPIO_PCTL_PC4_M | GPIO_PCTL_PC5_M);


    // Setup PWM timers to each of the GPIO
    GPIO_PORTC_PCTL_R |= GPIO_PCTL_PC4_M0PWM6 | GPIO_PCTL_PC5_M0PWM7;


    // Configure PWM module 0 to drive MOTORS       ***(TABLE 20-1 DATA SHEET)***

    // Right Motor : M0PWM6 (PC4)-> M0PWM3a
    // Left Motor  : M0PWM7 (PC5)-> M0PWM3b             (motion control module)

    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0;        // reset PWM0
    SYSCTL_SRPWM_R = 0;                                         // leave reset state

    PWM1_1_CTL_R = 0;                                // turn-off PWM1 generator 1 (drives outs 2 and 3)
    PWM0_3_CTL_R = 0;                                // turn-off PWM1 generator 3 (drives outs 6 and 7)

                                                                        // go high when comparator b is met
                                                                       // then drive low when the load value is reached

    PWM0_3_GENA_R = PWM_0_GENA_ACTCMPAD_ONE | PWM_0_GENA_ACTLOAD_ZERO; // output 6 on PWM1, gen 3a, cmpa

    PWM0_3_GENB_R = PWM_0_GENB_ACTCMPBD_ONE | PWM_0_GENB_ACTLOAD_ZERO; // output 7 on PWM1, gen 3b, cmpb

                                                      // set frequency to 40 MHz sys clock / 2 / 1024 = 19.53125 kHz
    PWM0_3_LOAD_R = 1024;                            // (internal counter counts down from load value to zero)

    PWM0_3_CMPA_R = 0;                               // off (0=always low, 1023=always high)
    PWM0_3_CMPB_R = 0;


    PWM0_3_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM1 generator 3

    // enable outputs
    PWM0_ENABLE_R = PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN;

}

void setpwm(uint16_t leftMotor, uint16_t rightMotor)
{

        PWM0_3_CMPA_R = rightMotor;     // PC4
        PWM0_3_CMPB_R = leftMotor;      // PC5

}




