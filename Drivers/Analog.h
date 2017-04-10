/*
 * Analog.h
 *
 *  Created on: 09/04/2017
 *      Author: otavi
 */

#ifndef DRIVERS_ANALOG_H_
#define DRIVERS_ANALOG_H_

#include "MKE02Z2.h"

typedef enum{
	PWM6 = 5u,
	PWM8 = 0u,
	PWM9 = 1u
} PWM;

void Analog_InitPWM(void);
void Analog_InitAnalog(void);

void Analog_SetPWMPin(PWM pwmPin);
void Analog_UnsetPWMPin(PWM pwmPin);
void Analog_Write(PWM pwmPin, uint16_t value);

#endif /* DRIVERS_ANALOG_H_ */
