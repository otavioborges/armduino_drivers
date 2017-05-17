/*
 ============================================================================
152000152000 Name        : main.c
 Author      : Otavio Borges
 Version     :
 Copyright   : nada
 Description : Hello World in C
 ============================================================================
 */

#include "MKE02Z2.h"
#include "BoardInit.h"

#include "Digital.h"
#include "Analog.h"
#include "TimedInt.h"
#include "lg_ac.h"

#define VOLTAGE_REFERENCE 4.83f
#define DESIRED_ROOM_TEMP	23u

uint32_t wait = 0;
uint8_t ligado = 0;

#define SAMPLES 32u
uint16_t temperatureSamples[SAMPLES];
uint32_t sampledValue;
uint8_t samplePosition = 0;
float temperature = 25.0f;

void TempReading(void){
	if(Analog_ReadComplete()){
		temperatureSamples[samplePosition] = Analog_ReadPolling(Analog0);
		samplePosition++;
		if(samplePosition == SAMPLES){
			sampledValue = 0;
			for(uint8_t index = 0; index < SAMPLES; index++)
				sampledValue += temperatureSamples[index];

			sampledValue = sampledValue >> 5; // 2^5 = 32 - x rsh y = x/(2^y);
			temperature = (sampledValue / ((float)ANALOG_MAX_VALUE)) * VOLTAGE_REFERENCE;
			temperature *= 100.0;

			samplePosition = 0;
		}
		Analog_StartReading(Analog0);
	}
}

// send command 3 times to confirm it got there!
void ToHot(void){
	if(LG_AC_State()){
		for(uint8_t resend = 0; resend < 8; resend++)
			LG_AC_SetTemperature(DESIRED_ROOM_TEMP);
	}else{
		for(uint8_t resend = 0; resend < 8; resend++)
			LG_AC_TurnOn(DESIRED_ROOM_TEMP);
	}
}

void ToCold(void){
	for(uint8_t resend = 0; resend < 8; resend++)
		LG_AC_TurnOff();
}

int main(void){
	ConfigBoardDefaultMuxing();

	Digital_pinMode(Digital8,INPUT_PULLUP);
	Digital_pinMode(DigitalLed,OUTPUT);

	Analog_InitAnalog();
	Analog_SetPin(Analog0);

	TimedInt_SetIRQFunction(TimedChannel1,TempReading);
	TimedInt_Config(TimedChannel1,5.0f);

	Analog_StartReading(Analog0);

	wait = Millis() + 3000u;
	while(1){
		if(Millis() > wait){
			if(temperature > DESIRED_ROOM_TEMP){
				ToHot();
			}else if(temperature < DESIRED_ROOM_TEMP - 3){
				ToCold();
			}

			wait = Millis() + 3000u;
		}
	}

	return 0;
}
