/*
 * lg_ac.c
 *
 *  Created on: 16/05/2017
 *      Author: Otavio
 */

#define TEMPERATURE_MIN			18u
#define TEMPERATURE_MAX			30u
#define TEMPERATURE_START_VALUE 0x30
#define TEMPERATURE_SKIP		0x10

#include "lg_ac.h"

uint8_t acState = 0;
uint8_t acTemp = 23u;

uint8_t convertTemperatureToCommand(uint8_t *temp){
	if((*temp > TEMPERATURE_MAX) || (*temp < TEMPERATURE_MIN))
		return 0;	// invalid temperature range

	uint8_t tempValue = *temp - TEMPERATURE_MIN;
	tempValue = TEMPERATURE_START_VALUE + (tempValue * TEMPERATURE_SKIP);

	*temp = tempValue;

	return 0x01;
}

void LG_AC_Init(void){
	NEC_IR_Init(PWM6,TimedChannel0);
}

uint8_t LG_AC_TurnOn(uint8_t startTemperature){
	uint8_t tempComm = startTemperature;
	if(!convertTemperatureToCommand(&tempComm))
		return 0;

	while(NEC_IR_ItensOnBuffer() >= NEC_IR_MAX_COMMANDS);
	NEC_IR_SendCommand(0x88,0x00,tempComm);

	acTemp = startTemperature;
	acState = 0x01;
	return 0x01;
}

void LG_AC_TurnOff(void){
	while(NEC_IR_ItensOnBuffer() >= NEC_IR_MAX_COMMANDS);
	NEC_IR_SendCommand(0x88,0xC0,0x05);

	acState = 0;
}

uint8_t LG_AC_State(void){
	return acState;
}

uint8_t LG_AC_SetTemperature(uint8_t temp){
	uint8_t tempComm = temp;
	if(!acState)
		return 0;	// ac off

	if(!convertTemperatureToCommand(&tempComm))
		return 0;

	while(NEC_IR_ItensOnBuffer() >= NEC_IR_MAX_COMMANDS);
	NEC_IR_SendCommand(0x88,0x08,tempComm);

	acTemp = temp;
	return 0x01;
}

uint8_t LG_AC_CurrentTemperature(void){
	return acTemp;
}
