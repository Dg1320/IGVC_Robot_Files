/*
 * my_wheel_ops.c
 *
 *  Created on: Nov 1, 2024
 *      Author: clutch13
 */

#include <stdint.h>
#include <stdbool.h>

#include "remote_control.h"
#include "tm4c123gh6pm.h"
#include "wait.h"

#define GREEN_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))

#define PIN_PD3 0b00001000

//  direction                                                JEEP WITH BOTH BATTERIES
#define RIGHT_MOTOR_OFFSET_SLOW_FWDBEGIN        65250
#define RIGHT_MOTOR_OFFSET_MEDIUM_FWDBEGIN      68250
#define RIGHT_MOTOR_OFFSET_FULL_SPEED_FWDBEGIN  72750

#define LEFT_MOTOR_OFFSET_SLOW_FWDBEGIN         65500
#define LEFT_MOTOR_OFFSET_MEDIUM_FWDBEGIN       68750
#define LEFT_MOTOR_OFFSET_FULL_SPEED_FWDBEGIN   80000

// reverse direction
#define RIGHT_MOTOR_OFFSET_SLOW_RVSBEGIN        56600
#define RIGHT_MOTOR_OFFSET_MEDIUM_RVSBEGIN      54100
#define RIGHT_MOTOR_OFFSET_FULL_SPEED_RVSBEGIN  48300

#define LEFT_MOTOR_OFFSET_SLOW_RVSBEGIN         56100
#define LEFT_MOTOR_OFFSET_MEDIUM_RVSBEGIN       53600
#define LEFT_MOTOR_OFFSET_FULL_SPEED_RVSBEGIN   40000


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
extern uint32_t rightMotorSpeed;
extern uint32_t leftMotorSpeed;

bool rightfwd = false;
bool leftfwd = false;
bool rightrvs = false;
bool leftrvs= false;
bool fwd = false;
bool rvs = false;

void rotateCounterClockwise(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed =LEFT_MOTOR_OFFSET_SLOW_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed =RIGHT_MOTOR_OFFSET_SLOW_RVSBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}
void rotateClockwise(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed =LEFT_MOTOR_OFFSET_SLOW_RVSBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed =RIGHT_MOTOR_OFFSET_SLOW_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}


void rightMotorIncreaseSpeedFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed +=500;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

}

void rightMotorIncreaseSpeedRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed -=50;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

}

void leftMotorIncreaseSpeedFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed +=50;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void leftMotorIncreaseSpeedRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed -=50;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void letsStop(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    WTIMER5_TAILR_R = 60000;                 // left motor pwm
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    TIMER2_TAILR_R = 60000;                 // right motor pwm
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    rightfwd = false;
    rightrvs = false;
    leftfwd = false;
    leftrvs = false;
    fwd = false;
    rvs = false;
}

void increaseForwardSpeed(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed +=50;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed +=50;;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}


void increaseReverseSpeed(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed -=500;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed -=500;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void goSlowRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_SLOW_RVSBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_SLOW_RVSBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = false;
    rvs = true;
    rightfwd = true;
    rightrvs = true;
    leftfwd = false;
    leftrvs = false;
}

void goSlowFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_SLOW_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_SLOW_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = true;
    rvs = false;
    rightfwd = true;
    rightrvs = false;
    leftfwd = true;
    leftrvs = false;
}


void goMediumFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_MEDIUM_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_MEDIUM_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = true;
    rvs = false;
    rightfwd = true;
    rightrvs = true;
    leftfwd = false;
    leftrvs = false;
}

void goMediumRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_MEDIUM_RVSBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_MEDIUM_RVSBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = false;
    rvs = true;
    rightfwd = false;
    rightrvs = true;
    leftfwd = false;
    leftrvs = true;
}

void goFullSpeedFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_FULL_SPEED_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_FULL_SPEED_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = true;
    rvs = false;
    rightfwd = true;
    rightrvs = false;
    leftfwd = true;
    leftrvs = false;
}

void goFullSpeedRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_FULL_SPEED_RVSBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_FULL_SPEED_RVSBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = false;
    rvs = true;
    rightfwd = false;
    rightrvs = true;
    leftfwd = false;
    leftrvs = true;
}
