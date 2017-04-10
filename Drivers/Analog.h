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

typedef enum{
	Analog0 = 11u,
	Analog1 = 10u,
	Analog2 = 9u,
	Analog3 = 8u,
	Analog4 = 7u,
	Analog5 = 6u
} AnalogPin;

typedef void (*functionPtr)(void);

void Analog_InitPWM(void);
void Analog_InitAnalog(void);

void Analog_DeinitPWM(void);
void Analog_DeinitAnalog(void);

void Analog_SetPWMPin(PWM pwmPin);
void Analog_UnsetPWMPin(PWM pwmPin);
void Analog_Write(PWM pwmPin, uint16_t value);

void Analog_EnableIRQ(void);
void Analog_DisableIRQ(void);
void Analog_SetIRQFunction(functionPtr function);
void Analog_SetPin(AnalogPin pin);
void Analog_UnsetPin(AnalogPin pin);
uint16_t Analog_ReadPolling(AnalogPin pin);
void Analog_ReadIRQStart(AnalogPin pin);
uint16_t Analog_ReadIRQValue(AnalogPin pin);

#endif /* DRIVERS_ANALOG_H_ */
