/*
 * my_wheel_ops.c
 *
 *  Created on: Mar 30, 2024
 *      Author: clutch13
 */

#include <stdint.h>
#include <stdbool.h>

#include "remote_control.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "my_wheel_ops.h"
#include "pwm_init.h"


int32_t right_pwm = 0;
int32_t left_pwm = 0;

extern bool valid;
extern bool forward;
extern bool reverse;
extern bool cw;
extern bool ccw;
extern bool stop;


#define RIGHT_RECIEVER  0b00000010
#define LEFT_RECIEVER   0b00000001

//#define DEBUG


void go_forward ( uint8_t speed)
{

               switch(speed)                                 // control speed--- thousand millimeters per minute
            {

                case 2:
                        left_pwm = 770-5;
                        right_pwm = 770;
                        setpwm(left_pwm, right_pwm);
                        break;

                case 3: left_pwm = 820-5;
                        right_pwm = 820;
                        setpwm(left_pwm, right_pwm);
                        break;

                case 4: left_pwm = 870-5;
                        right_pwm = 870;
                        setpwm(left_pwm, right_pwm);
                        break;

                case 5: left_pwm = 920-5;
                        right_pwm = 920;
                        setpwm(left_pwm, right_pwm);
                        break;

                case 6: left_pwm = 970-5;
                        right_pwm = 970;
                        setpwm(left_pwm, right_pwm);
                        break;

                case 7: left_pwm = 1018-5;
                        right_pwm = 1018;
                        setpwm(left_pwm, right_pwm);
                        break;


                default: left_pwm = 1018-5;
                        right_pwm = 1018;
                        setpwm(left_pwm, right_pwm);

            }




}

void go_backwards( uint8_t speed )
{

               switch(speed)                                 // control speed--- thousand millimeters per minute
            {
                        case 2:
                                left_pwm = 770-25;
                                right_pwm = 770;
                                setpwm(left_pwm, right_pwm);
                                break;

                        case 3: left_pwm = 825-25;
                                right_pwm = 825;
                                setpwm(left_pwm, right_pwm);
                                break;

                        case 4: left_pwm = 875-25;
                                right_pwm = 875;
                                setpwm(left_pwm, right_pwm);
                                break;

                        case 5: left_pwm = 935-25;
                                right_pwm = 935;
                                setpwm(left_pwm, right_pwm);
                                break;

                        case 6: left_pwm = 980-25;
                                right_pwm = 980;
                                setpwm(left_pwm, right_pwm);
                                break;

                        case 7: left_pwm = 1018-25;
                                right_pwm = 1018;
                                setpwm(left_pwm, right_pwm);
                                break;

                        default: left_pwm = 1018-25;
                                right_pwm = 1018;
                                setpwm(left_pwm, right_pwm);

            }




}

void go_right( uint8_t speed)
{

                left_pwm = 970-5;
                right_pwm = 970;
                setpwm(left_pwm, right_pwm);


}

void go_left (uint8_t speed)
{
            left_pwm = 970-5;
            right_pwm = 970;
            setpwm(left_pwm, right_pwm);



}

void lets_stop (void)
{
            setpwm(1,1);

}

