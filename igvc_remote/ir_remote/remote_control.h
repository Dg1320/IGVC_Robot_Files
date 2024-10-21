/*
 * remote_control.h
 *
 *  Created on: Apr 12, 2024
 *      Author: clutch13
 */

#ifndef REMOTE_CONTROL_H_
#define REMOTE_CONTROL_H_


#include <stdint.h>


void button_complete(void);
void button_command_ISR(void);
void setup_remote_functions(void);


#endif /* REMOTE_CONTROL_H_ */
