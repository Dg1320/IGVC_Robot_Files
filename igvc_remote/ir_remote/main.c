/*
 * main.c
 *
 *  Created on: Oct 19, 2024
 *      Author: clutch13
 */



//
//

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------
//
// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz
//
// Hardware configuration:

// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1
//
//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "clock.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"
#include "remote_control.h"
#include "wait.h"
#include "timing.h"
#include "isr.h"
// Bitband alias
#define GREEN_LED       (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define BLUE_LED        (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))

// PortF masks
#define GREEN_LED_MASK 0b00001000
#define BLUE_LED_MASK 0b00000100

#define FORWARD  0b00000001
#define STOP     0b00000010
#define RIGHT    0b00000100
#define LEFT     0b01000000
#define SLOW     0b00000010
#define CLOCKWISE           0b00000001
#define COUNTERCLOCKWISE    0b00000010

#define PC4_MASK  0b00010000
#define PC5_MASK  0b00100000

//#define DEBUG


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
   // signals from esp32
    // will need four signals to determine fw/stop/l/r
 void esp32_signals_setup(void)
 {
        GPIO_PORTD_DIR_R &=  ~(FORWARD | STOP | RIGHT | LEFT);  // PD[0] and PD[1] PD[2] and PD[6] are inputs
        GPIO_PORTD_DEN_R |= FORWARD | STOP | RIGHT | LEFT ;
        GPIO_PORTD_PDR_R |= FORWARD | STOP | RIGHT | LEFT ;                 // enable internal pull-down resistors

        // set up interrupt
        GPIO_PORTD_IM_R &=  ~(FORWARD | STOP | RIGHT | LEFT);    // clear interrupt mask to prevent false interrupts
        GPIO_PORTD_IBE_R &= ~(FORWARD | STOP | RIGHT | LEFT);    // Turn off both edge detecting sense
        GPIO_PORTD_IEV_R |=   FORWARD | STOP | RIGHT | LEFT ;    // Make detect rising edge
        GPIO_PORTD_ICR_R |=   FORWARD | STOP | RIGHT | LEFT ;    // clear the interrupts
        GPIO_PORTD_IM_R  |=   FORWARD | STOP | RIGHT | LEFT ;    // turn on interrupt mask

        NVIC_EN0_R = 1 << (INT_GPIOD-16);                // turn-on interrupt 19 (GPIOD)

        GPIO_PORTF_DIR_R &=  ~SLOW;  // PD[0] and PD[1] PD[2] and PD[6] are inputs
        GPIO_PORTF_DEN_R |= SLOW;
        GPIO_PORTF_PDR_R |= SLOW ;                 // enable internal pull-down resistors


        // set up interrupt
        GPIO_PORTF_IM_R &=  ~SLOW;    // clear interrupt mask to prevent false interrupts
        GPIO_PORTF_IBE_R &= ~SLOW;    // Turn off both edge detecting sense
        GPIO_PORTF_IEV_R |=   SLOW ;    // Make detect rising edge
        GPIO_PORTF_ICR_R |=  SLOW ;    // clear the interrupts
        GPIO_PORTF_IM_R  |=   SLOW ;    // turn on interrupt mask
        NVIC_EN0_R = 1 << (INT_GPIOF-16);                // turn-on interrupt 19 (GPIOF)

}




void initHw()
{
        // Initialize system clock to 40 MHz
        initSystemClockTo40Mhz();

        // Enable clocks
        SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R5;
        _delay_cycles(3);

        // Configure LED pins
       GPIO_PORTF_DIR_R |= GREEN_LED_MASK | BLUE_LED_MASK ;        // output
       GPIO_PORTF_DEN_R |= GREEN_LED_MASK | BLUE_LED_MASK;  // enable LED

       GPIO_PORTC_DIR_R |= PC4_MASK | PC5_MASK ;        // output
       GPIO_PORTC_DEN_R |= PC4_MASK | PC5_MASK;         // enable outputs

       initUart0();

}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

extern bool waitNow;
extern bool clockWiseWaitNow;
extern bool counterClockwiseWaitNow;

int main(void)

{
    USER_DATA data;
    initHw();
    initSysTick();
    oneShotSetup();
    setup_remote_functions();
    oneSecTimerSetup();

    // Setup UART0 baud rate
    setUart0BaudRate(115200, 40e6);

    esp32_signals_setup();


    while(1)
    {
        if(waitNow)
        {
            waitMicrosecond(1000000);
            waitNow = false;

            letsStop();
        }
        if(clockWiseWaitNow)
        {
            waitMicrosecond(1000000);
            clockWiseWaitNow = false;

            letsStop();
        }
        if(counterClockwiseWaitNow)
        {
            waitMicrosecond(1000000);
            counterClockwiseWaitNow = false;

            letsStop();
        }

    }
}




