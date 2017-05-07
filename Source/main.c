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
#include "Tone.h"
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

void ConnectedTone(void){
	Tone_Note(Re,2);
	Tone_Note(MiBemol,2);
	Tone_Note(Fa,6);
}

void DisconnectedTone(void){
	Tone_Note(Re,2);
	Tone_Note(MiBemol,2);
	Tone_Note(Do,6);
}

#define SPI_READ  0x80
#define SPI_WRITE 0x00

uint8_t dataOut[2];
uint8_t dataIn[2];

int main(void){
	ConfigBoardDefaultMuxing();

	Tone_Init(TimedChannel1,Digital7);
	Tone_Time(50);

	Digital_pinMode(Digital2,INPUT);

	Digital_pinMode(Digital3,OUTPUT);
	Digital_pinMode(DigitalLed,OUTPUT);

	SPI_Init(1000000,Master,ActiveHigh,Middle,MSB);

	Digital_Write(Digital3,LOW);
	Delay(1000);
	Digital_Write(Digital3,HIGH);
	Delay(2000);					// Reset interface

	dataOut[0] = SPI_READ | (0x37 << 1);
	dataOut[1] = 0;
	SPI_Transfer(dataOut,dataIn,2);

	while(1){

	}

	return 0;
}
