/*
 * remote_control.c
 *
 *  Created on: Apr 12, 2024
 *      Author: clutch13
 */



#include <stdint.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "remote_control.h"
#include "uart0.h"
#include "wait.h"
#include "my_wheel_ops.h"

#define GREEN_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define RIGHT_MOTOR            (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 4*4)))    // PC[4]
#define LEFT_MOTOR            (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 5*4)))     // PC[5]
#define BLUE_LED        (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))


#define PIN_PD3 0b00001000
// forward direction                                                JEEP WITH BOTH BATTERIES
#define RIGHT_MOTOR_OFFSET_FWDBEGIN 64300                              // right motor starts moving forward at 64300

#define LEFT_MOTOR_OFFSET_FWDBEGIN 65200                               // left motor starts moving forward at 65200

// reverse direction
#define RIGHT_MOTOR_OFFSET_RVSBEGIN 57400                              // right motor starts going backwards at 57400

#define LEFT_MOTOR_OFFSET_RVSBEGIN 55600                               // left motor starts moving forward at 55600

#define TAU 22500                                                      // time of tau/ 25ns = count time
#define IDLE_LOW_LIMIT   480000
#define IDLE_HIGH_LIMIT  560000
#define THIRTY_TWO_BIT_PLACE    4294967296

#define GO   0b00000001
#define STOP 0b00000010
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
   uint32_t button_info = 0;
   uint32_t time = 0;
   uint32_t prev_time = 0;
   uint32_t bit_count = 0;
   uint8_t state = 0 ;
   int32_t remote_command = 0;


   uint32_t rightMotorSpeed = 60000;
   uint32_t leftMotorSpeed = 60000;
extern   bool rightfwd;
extern   bool leftfwd;
extern   bool rightrvs;
extern   bool leftrvs;
extern   bool fwd;
extern   bool rvs;
extern bool self_drive;



void setup_remote_functions(void)
{
        SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R3;
        _delay_cycles(3);

       GPIO_PORTD_DIR_R &=  ~PIN_PD3;                  // IR LED
       GPIO_PORTD_DEN_R |= PIN_PD3 ;

       GPIO_PORTD_AFSEL_R |= PIN_PD3;                   // choose alternative function for GPIO
       GPIO_PORTD_PCTL_R &= ~(GPIO_PCTL_PD3_M);        // clear original function
       GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD3_WT3CCP1;     // use as widetimer

       // -------- Set up edge-time mode @ 40MHz for controller  PD[3] for controller

       WTIMER3_CTL_R &= ~TIMER_CTL_TBEN;                         // turn-off counter before reconfiguring
       WTIMER3_CFG_R = 4;                                        // configure as 32-bit counter
                                                                 // TnCMR == 1 and TnMR == 3 then set direction    // PAGE 724
       WTIMER3_TBMR_R |= TIMER_TBMR_TBMR_CAP ;                   //  capture mode / Edge time mode / count UP
       WTIMER3_TBMR_R |= TIMER_TBMR_TBCMR |TIMER_TBMR_TBCDIR ;
       WTIMER3_CTL_R |= TIMER_CTL_TBEVENT_NEG;                   // set falling edge capture event
       WTIMER3_IMR_R = TIMER_IMR_CBEIM;                          // Enable capture interrupts
       WTIMER3_ICR_R |= TIMER_ICR_CBECINT ;                      // Clear Capture interrupt flag
       NVIC_EN3_R = 1 << (INT_WTIMER3B-16-96);                   // turn-on interrupt 116 (TIMER3A)   --- TABLE 2-9 & 3-8

       WTIMER3_CTL_R |= TIMER_CTL_TBEN;                          // turn-on timer


}



void esp32_signals(void)
{
    if(GPIO_PORTD_MIS_R & GO)       // left reciever
   {

           BLUE_LED ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;        //  turn-off timer to set new time
           leftMotorSpeed = LEFT_MOTOR_OFFSET_FWDBEGIN+1500;
           WTIMER5_TAILR_R = leftMotorSpeed;
           WTIMER5_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

           TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
           rightMotorSpeed = RIGHT_MOTOR_OFFSET_FWDBEGIN+1500;
           TIMER2_TAILR_R = rightMotorSpeed;
           TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

           fwd = true;
           rvs = false;

   }


   if(GPIO_PORTD_MIS_R & STOP)       // left reciever
   {
           BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
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
       GPIO_PORTD_ICR_R |= STOP | GO;       // clear the interrupt flag


}
//#define DEBUG




void button_complete(void)
{


            switch(remote_command)
            {

                case 0b00100000110111110000001011111101:           // forward       UP BUTTON


                            if (!fwd)  goForward();

                            else  increaseForwardSpeed();

                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111111000001001111101:          // backwards      DOWN BUTTON

                            if (!rvs) goReverse();

                            else increaseReverseSpeed();

                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111110110000010011111:            //    RIGHT BUTTON

                            GREEN_LED = 1;
                            waitMicrosecond(500000);                      // allow light to remain .... extra signals can be caught from different sources

                            break;

                case 0b00100000110111111110000000011111:            //    LEFT BUTTON

                            GREEN_LED = 1;
                            waitMicrosecond(500000);

                            break;


                case 0b00100000110111110000000011111111:         //     CHANNEL UP BUTTON

                            if (!rightfwd) rightMotorStartFwd();

                            else rightMotorIncreaseSpeedFwd();

                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111111000000001111111:      //      CHANNEL DOWN BUTTON

                            if (!rightrvs)  rightMotorStartRvs();

                            else rightMotorIncreaseSpeedRvs();

                            break;

//------------------------------------------------------------------------------------

                case 0b00100000110111110100000010111111:    //     VOLUME UP BUTTON


                            if (!leftfwd) leftMotorStartFwd();

                            else leftMotorIncreaseSpeedFwd();

                            break;
//------------------------------------------------------------------------------------
                case 0b00100000110111111100000000111111:    //    VOLUME DOWN BUTTON


                            if (!leftrvs)   leftMotorStartRvs();

                            else leftMotorIncreaseSpeedRvs();

                            break;


//------------------------------------------------------------------------------------
                case 0b00100000110111110110101010010101:  // stop         NETFLIX BUTTON

                            letsStop();

                            break;

            }

            char mySpeed[9];
            putsUart0("Left  wheel speed: ")
            int_to_ascii(leftMotorSpeed,mySpeed);
            putsUart0(mySpeed);
            putsUart0("\n");

            putsUart0("Right wheel speed: ");
            int_to_ascii(rightMotorSpeed,mySpeed);
            putsUart0(mySpeed);
            putsUart0("\n");
            remote_command = 0;

}

void button_command_ISR(void)
{

     // loop until the entire command is complete
     // but first check for failures
     //  WTIMER3_TBR_R   <--- register where values are placed when falling edge occurs

#ifdef DEBUG

    number_to_uart(time);
    putsUart0(" \n");

#endif

      time = WTIMER3_TBV_R - prev_time;

      prev_time = WTIMER3_TBV_R;


       if(state == 0 )
       {

               button_info = 0;
               bit_count = 32;
               prev_time = 0;
               time = 0;
               WTIMER3_TBV_R = 0;
               state++;

       }


       else if (state == 1)
      {
            if ((time > IDLE_LOW_LIMIT) && (time < IDLE_HIGH_LIMIT))
            {
                   state++;
            }

            else
                {state = 0;
                 button_info = 0;
               bit_count = 32;

               time = 0;


                }
      }


     else                          //START READING REMOTE CODE
    {


           if ((time > TAU) && (time < (3*TAU) ) )                         // detected a zero?
           {
                  if(bit_count > 0)
                  {
                      bit_count --;         // recieving the first bit of info

                  }
           }

           else if((time > (3*TAU) )&&  (time < (5*TAU) ))         // detected a one?
           {

                      if (bit_count > 0)
                  {
                          bit_count --;

                          button_info |= 1 << bit_count;  // place value in the button code


                  }

            }
            else if (bit_count == 0)            // reached the final bit
            {
                      remote_command = button_info;             // send final result to global value

                      prev_time = 0;
                      time = 0;
                      WTIMER3_TBV_R = 0;

                      bit_count = 32;
                      button_complete();
            }

            else
            {
               button_info = 0;
                bit_count = 32;
                prev_time = 0;
                time = 0;
               WTIMER3_TBV_R = 0;

               GREEN_LED = 0;


            }



   }

    WTIMER3_ICR_R |= TIMER_ICR_CBECINT ;                      // Clear Capture interrupt flag
     GPIO_PORTD_ICR_R |= PIN_PD3;     // clear the interrupts


}


