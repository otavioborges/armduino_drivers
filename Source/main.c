/*
 ============================================================================
 Name        : main.c
 Author      : Otavio Borges
 Version     :
 Copyright   : nada
 Description : Hello World in C
 ============================================================================
 */

#include "MKE02Z2.h"
#include "BoardInit.h"
#include "Analog.h"

/*
 *
 * Print a greeting message on standard output and exit.
 *
 * On embedded platforms this might require semi-hosting or similar.
 *
 * For example, for toolchains derived from GNU Tools for Embedded,
 * to enable semi-hosting, the following was added to the linker:
 *
 * --specs=rdimon.specs -Wl,--start-group -lgcc -lc -lc -lm -lrdimon -Wl,--end-group
 *
 * Adjust it for other toolchains.
 *
 */

uint32_t countP = 0;
uint16_t value = 0;

uint8_t dataIn[4];
uint8_t dataOut[4];

uint8_t rising = 1;

int main(void){
	ConfigBoardDefaultMuxing();

	Analog_InitPWM();
	Analog_SetPWMPin(PWM8,TRUE);

	countP = Millis() + 1u;
	while(1){
		if(Millis() > countP){
			Analog_Write(PWM8,value);

			countP = Millis() + 1u;
			value += 10u;

			if(value > 0x7FFF)
				value = 0;
		}
	}

	return 0;
}
