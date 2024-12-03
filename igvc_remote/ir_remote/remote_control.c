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

#define TAU 22500                                                      // time of tau/ 25ns = count time
#define IDLE_LOW_LIMIT   480000
#define IDLE_HIGH_LIMIT  560000
#define THIRTY_TWO_BIT_PLACE    4294967296

#define FORWARD  0b00000001
#define STOP     0b00000010
#define RIGHT    0b00000100
#define LEFT     0b01000000
#define SLOW     0b00000010
#define CLOCKWISE           0b00000001
#define COUNTERCLOCKWISE    0b00000010

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

bool waitNow = false;
bool clockWiseWaitNow = false;
bool counterClockwiseWaitNow = false;

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

void rightMotorIncreaseSpeedFwdFromISR(void)
{
    GREEN_LED = 1;
    waitMicrosecond(500000);
    //////////////////////////////////////////////////////////////////////////
    TIMER2_CTL_R &= ~TIMER_CTL_TAEN;             // turn-off timer to set new time
    rightMotorSpeed +=50;
    TIMER2_TAILR_R = rightMotorSpeed;
    TIMER2_CTL_R |= TIMER_CTL_TAEN;         // turn-on one shot timer

}


void esp32_signals(void)
{
    if(GPIO_PORTD_MIS_R & FORWARD)       // forward reciever
   {

           BLUE_LED ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           goMediumFwd();
           waitNow = true;

   }


   if(GPIO_PORTD_MIS_R & STOP)       // stop reciever
   {
           BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           letsStop();

   }

   if(GPIO_PORTD_MIS_R & RIGHT)       // right reciever
   {
           BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           rotateClockwise();
           clockWiseWaitNow = true;

   }
   if(GPIO_PORTD_MIS_R & LEFT)       // right reciever
   {
           BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           rotateCounterClockwise();          // speed up the left motor to turn to the right
           counterClockwiseWaitNow = true;

   }
   GPIO_PORTD_ICR_R |= STOP | FORWARD | RIGHT | LEFT;       // clear the interrupt flag


}


void esp32_slowSignal(void)
{
           BLUE_LED ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
           increaseReverseSpeed();

   GPIO_PORTF_ICR_R |= SLOW;       // clear the interrupt flag


}
//
//void esp32_rotateSignal(void)
//{
//    if(GPIO_PORTE_MIS_R & CLOCKWISE)       // CLOCKWISE reciever
//    {
//        BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
//        rotateClockwise();
//    }
//    if(GPIO_PORTE_MIS_R & COUNTERCLOCKWISE)       // COUNTERCLOCKWISE reciever
//    {
//        BLUE_LED  ^= 1;                    // WHEN INTERRUPT OCCUR TOGGLE LED
//        rotateCounterClockwise();
//    }
//    GPIO_PORTE_ICR_R |= CLOCKWISE | COUNTERCLOCKWISE ;       // clear the interrupt flag}
//}


//#define DEBUG




void button_complete(void)
{


            switch(remote_command)
            {

                case 0b00100000110111110000001011111101:           // forward       UP BUTTON


                            if (!fwd)  goSlowFwd();

                            else  increaseForwardSpeed();

                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111111000001001111101:          // backwards      DOWN BUTTON

                            if (!rvs) goSlowRvs();

                            else increaseReverseSpeed();

                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111110110000010011111:            //    RIGHT BUTTON

                            rightMotorIncreaseSpeedFwd();

                            break;

                case 0b00100000110111111110000000011111:            //    LEFT BUTTON

                            leftMotorIncreaseSpeedRvs();

                            break;

                case 0b00100000110111110000000011111111:         //     CHANNEL UP BUTTON

                            goFullSpeedFwd();


                            break;
//------------------------------------------------------------------------------------

                case 0b00100000110111111000000001111111:      //      CHANNEL DOWN BUTTON

                            goFullSpeedRvs();

                            break;

//------------------------------------------------------------------------------------

                case 0b00100000110111110100000010111111:    //     VOLUME UP BUTTON


                            goMediumFwd();

                            break;
//------------------------------------------------------------------------------------
                case 0b00100000110111111100000000111111:    //    VOLUME DOWN BUTTON

                            goMediumRvs();

                            break;


//------------------------------------------------------------------------------------
                case 0b00100000110111110110101010010101:  // stop         NETFLIX BUTTON

                            letsStop();

                            break;

            }

            char mySpeed[9];
            putsUart0("Left  wheel speed: ");
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


