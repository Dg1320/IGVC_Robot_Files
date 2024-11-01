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
// forward direction                                                JEEP WITH BOTH BATTERIES
#define RIGHT_MOTOR_OFFSET_FWDBEGIN 64300                              // right motor starts moving forward at 64300

#define LEFT_MOTOR_OFFSET_FWDBEGIN 65200                               // left motor starts moving forward at 65200

// reverse direction
#define RIGHT_MOTOR_OFFSET_RVSBEGIN 57400                              // right motor starts going backwards at 57400

#define LEFT_MOTOR_OFFSET_RVSBEGIN 55600                               // left motor starts moving forward at 55600

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


void rightMotorStartFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    rightfwd = true;
    rightrvs = false;
    fwd = false;
    rvs = false;
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
void rightMotorStartRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_RVSBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    rightfwd = false;
    rightrvs = true;
    fwd = false;
    rvs = false;
}
void rightMotorIncreaseSpeedRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed -=500;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

}
void leftMotorStartFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    leftfwd = true;
    leftrvs = false;
    fwd = false;
    rvs = false;
}
void leftMotorIncreaseSpeedFwd(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed +=500;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void leftMotorStartRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_RVSBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    leftfwd = false;
    leftrvs = true;
    fwd = false;
    rvs = false;

}
void leftMotorIncreaseSpeedRvs(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed -=500;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void letsStop(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = 60000;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    TIMER2_TAILR_R = 60000;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    rightfwd = false;
    rightrvs = false;
    leftfwd = false;
    leftrvs = false;
    fwd = false;
    rvs = false;
}

void goForward(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed = LEFT_MOTOR_OFFSET_FWDBEGIN;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_FWDBEGIN;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    fwd = true;
    rvs = false;
}
void increaseForwardSpeed(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    leftMotorSpeed +=1000;
    WTIMER5_TAILR_R = leftMotorSpeed;
    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed +=1000;;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}

void goReverse(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time

    leftMotorSpeed = LEFT_MOTOR_OFFSET_RVSBEGIN;
    rightMotorSpeed = RIGHT_MOTOR_OFFSET_RVSBEGIN;

    WTIMER5_TAILR_R = leftMotorSpeed;
    TIMER2_TAILR_R = rightMotorSpeed;

    WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

    rvs = true;
    fwd = false;

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
    rightMotorSpeed -=1000;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer
}
