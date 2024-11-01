/*
 * my_wheel_ops.h
 *
 *  Created on: Mar 30, 2024
 *      Author: clutch13
 */

#ifndef MY_WHEEL_OPS_H_
#define MY_WHEEL_OPS_H_

#include <stdint.h>
void rightMotorStartFwd(void);
void rightMotorIncreaseSpeedFwd(void);
void rightMotorStartRvs(void);
void rightMotorIncreaseSpeedRvs(void);

void leftMotorStartFwd(void);
void leftMotorIncreaseSpeedFwd(void);
void leftMotorStartRvs(void);
void leftMotorIncreaseSpeedRvs(void);

void goForward(void);
void goReverse(void);

void increaseForwardSpeed(void);
void increaseReverseSpeed(void);


void letsStop(void);

#endif /* MY_WHEEL_OPS_H_ */
