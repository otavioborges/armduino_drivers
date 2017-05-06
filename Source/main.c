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
#include "Digital.h"
#include "Interrupt.h"
#include "SPI.h"

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

void DigIrqFunction(uint8_t peripehral){
	countP = 10;
}

uint8_t dataIn[4];
uint8_t dataOut[4];

int main(void){
	ConfigBoardDefaultMuxing();

	Digital_pinMode(Digital7,INPUT_PULLUP);
	Digital_pinMode(Digital8,OUTPUT);

	Digital_pinMode(DigitalLed,OUTPUT);

	Interrupt_SetIRQFunction(DigIrqFunction);
	Interrupt_Init(TRUE, Edges);
	Interrupt_EnablePin(IntPin11,FallingLowLevel);

	SPI_Init(100000u, Master, ActiveHigh, Middle, MSB);

	Digital_Write(Digital8,LOW);	// resets device
	Delay(10);
	Digital_Write(Digital8,HIGH);
	Delay(10);						// reset done!

	dataOut[0] = 0b01011111;
	dataOut[1] = 0x03;
	SPI_Transfer(dataOut,dataIn,4); // go to bank 3

	dataOut[0] = 0b00010101;
	SPI_Transfer(dataOut,dataIn,4); // read ECOCON

	countP = Millis() + 1000u;
	while(1){
		if(Millis() > countP){
			Digital_Toggle(DigitalLed);
			countP = Millis() + 1000u;
		}
	}

	return 0;
}
