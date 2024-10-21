/*
 * pwminit.h
 *
 *  Created on: Mar 13, 2024
 *      Author: clutch13
 */

#ifndef PWM_INIT_H_
#define PWM_INIT_H_


#include <stdint.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void pwm_init();
void setpwm(uint16_t leftMotor, uint16_t rightMotor);

#endif /* PWM_INIT_H_ */
