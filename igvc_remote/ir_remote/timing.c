/*
 * timing.c
 *
 *  Created on: Nov 1, 2024
 *      Author: clutch13
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

   extern uint32_t rightMotorSpeed;
   extern uint32_t leftMotorSpeed ;

void oneSecTimerSetup(void)
{
    // Timer one Setup
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    _delay_cycles(3);
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER1_TAILR_R = 40000000;                       // set load value to 40e6 for 1 Hz interrupt rate
    TIMER1_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    TIMER1_CTL_R |= TIMER_CTL_TAEN;                  // turn-on timer
    NVIC_EN0_R = 1 << (INT_TIMER1A-16);              // turn-on interrupt 37 (TIMER1A)
}



void oneShotSetup()
{
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2 ;       // turn on timer 2
    _delay_cycles(3);
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER2_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER2_TAMR_R |= TIMER_TAMR_TAMR_1_SHOT;         // one shot mode
    TIMER2_TAMR_R &= ~TIMER_TAMR_TACDIR;             // (count down)
    TIMER2_TAILR_R = rightMotorSpeed;                       // set load value for interrupt every 10ms  == 40Hz
    TIMER2_IMR_R = TIMER_IMR_TATOIM;                  // turn-on interrupts
    NVIC_EN0_R = 1 << (INT_TIMER2A-16);              // turn-on interrupt 39 (TIMER2A)   --- TABLE 2-9 & 3-8

    // wide timer 5A     PD[6]
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R5;
    _delay_cycles(3);
     WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off Wide timer A before reconfiguring
     WTIMER5_CFG_R = 4;                                  // configure as 32-bit timer
     WTIMER5_TAMR_R |= TIMER_TAMR_TAMR_1_SHOT;         // one shot mode on timer A
     WTIMER5_TAMR_R &= ~TIMER_TAMR_TACDIR;             // (count down)
     WTIMER5_TAILR_R = leftMotorSpeed;                       // set load value for interrupt every 10ms  == 40Hz
     WTIMER5_IMR_R = TIMER_IMR_TATOIM;                  // turn-on interrupts
     NVIC_EN3_R = 1 << (INT_WTIMER5A-16-96);              // turn-on interrupt 120 (TIMER5A)   --- TABLE 2-9 & 3-8
}

void initSysTick()
{
    // sysTick initialized for 20ms
    NVIC_INT_CTRL_R &= NVIC_INT_CTRL_PEND_SV;
    NVIC_ST_CTRL_R = 0;                                                 // disable systick while configuring
    NVIC_ST_RELOAD_R = 800000 - 1;                                       // reload value set to be 1ms based on 40MHz clock
                                                                        // datasheet pg.140 on why subtracting 1
    NVIC_ST_CURRENT_R = 0 ;                                             // any write to current clears count bit which activates the interrupt
    NVIC_ST_CTRL_R = 0x0007;                                            // enable systick x interrupt x systemclock

}





