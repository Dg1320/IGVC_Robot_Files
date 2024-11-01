/*
 * isr.c
 *
 *  Created on: Nov 1, 2024
 *      Author: clutch13
 */
#include <inttypes.h>
#include "tm4c123gh6pm.h"

#define RIGHT_MOTOR     (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 4*4)))    // PC[4]
#define LEFT_MOTOR      (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 5*4)))    // PC[5]

 uint8_t seconds = 0;

void sysTickISR()
{
// make sure the pwm is 20ms every time sow we will turn on the signal here
    RIGHT_MOTOR = 1;
    LEFT_MOTOR = 1;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;        // turn-on one shot timer
}

void oneShotISR()
{
    RIGHT_MOTOR = 0;
    TIMER2_ICR_R = TIMER_ICR_TATOCINT;           // clear interrupt flag
}
void oneShotISR2()
{
    LEFT_MOTOR = 0;
    WTIMER5_ICR_R = TIMER_ICR_TATOCINT;           // clear interrupt flag
}

void oneSecISR()
{
    if(seconds< 5) seconds++;

    else if (seconds==5)
    {
        seconds = 0;

    }
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;           // clear interrupt flag
}
