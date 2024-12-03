/*
 * my_wheel_ops.h
 *
 *  Created on: Mar 30, 2024
 *      Author: clutch13
 */

#ifndef MY_WHEEL_OPS_H_
#define MY_WHEEL_OPS_H_

#include <stdint.h>
void rightMotorIncreaseSpeedFwd(void);
void rightMotorIncreaseSpeedRvs(void);

void leftMotorIncreaseSpeedFwd(void);
void leftMotorIncreaseSpeedRvs(void);

void increaseForwardSpeed(void);
void increaseReverseSpeed(void);

void goSlowFwd(void);
void goSlowRvs(void);

void goMediumFwd(void);
void goMediumRvs(void);

void goFullSpeedFwd(void);
void goFullSpeedRvs(void);

void rotateClockwise(void);
void rotateCounterClockwise(void);

void letsStop(void);

#endif /* MY_WHEEL_OPS_H_ */
