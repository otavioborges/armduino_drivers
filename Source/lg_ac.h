/*
 * lg_ac.h
 *
 *  Created on: 16/05/2017
 *      Author: Otavio
 */

#ifndef SOURCE_LG_AC_H_
#define SOURCE_LG_AC_H_

#include "Analog.h"
#include "TimedInt.h"
#include "NEC_IR.h"

void LG_AC_Init(void);
uint8_t LG_AC_TurnOn(uint8_t startTemperature);
void LG_AC_TurnOff(void);
uint8_t LG_AC_State(void);

uint8_t LG_AC_SetTemperature(uint8_t temp);
uint8_t LG_AC_CurrentTemperature(void);

#endif /* SOURCE_LG_AC_H_ */
