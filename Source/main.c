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

#include "Serial.h"
#include <string.h>

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

uint8_t buffer[10];
uint8_t bufferPos = 0;

void Serial0Irq(void){
	Serial_Read(Serial0,buffer,10);
	asm("nop");
}

uint8_t msg[8];

int main(void){
	ConfigBoardDefaultMuxing();

	strcpy(msg,"Hello\r\n");

	Serial_Setup(Serial0,57600u,None,One);
	Serial_SetIRQFunction(Serial0,Serial0Irq);
	Serial_EnableReceiveIRQ(Serial0);

	Digital_pinMode(Digital2,OUTPUT);

	for(;;){
		// loop routine
		Delay(1000u);
		Serial_Write(Serial0,msg,7);
	}

	return 0;
}
