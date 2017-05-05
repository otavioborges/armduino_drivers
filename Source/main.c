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
//#include "I2C.h"
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

#define compassAddr 0x1E

uint8_t msg[20];
int16_t measureX,measureZ,measureY;

uint8_t uartMsg[100];
volatile uint16_t pos = 0;

void SerialRecv(void){
	Serial_Read(Serial0,uartMsg,1);
	pos++;
}

uint8_t sendBuf[10];

int main(void){
	ConfigBoardDefaultMuxing();

	Serial_Setup(Serial0,57600u,None,One);
	Serial_SetIRQFunction(Serial0,SerialRecv);
	Serial_EnableReceiveIRQ(Serial0);

	strcpy(sendBuf,"ARMduino\n");

	for(;;){
		if(uartMsg[(pos-1)] == '\n'){
			__asm("nop");
			pos = 0;
		}else{
			Serial_Write(Serial0,sendBuf,9);
			Delay(1000);
		}
	}

	/*I2C_Init(I2CMaster,StandardMode,0,0,0);
	msg[0] = 0x00;
	msg[1] = 0b01111000;
	I2C_MasterSendMsg(compassAddr,msg,2);
	msg[0] = 0x01;
	msg[1] = 0;
	I2C_MasterSendMsg(compassAddr,msg,2);
	msg[0] = 0x02;
	msg[1] = 0;
	I2C_MasterSendMsg(compassAddr,msg,2);

	Delay(6);
	for(;;){
		I2C_MasterWrite(0x3D);
		I2C_MasterWrite(0x06);
		for(uint8_t pos = 0; pos < 6; pos++){
			msg[pos] = I2C_MasterRead(0x01);
		}
		measureX = msg[0];
		measureX = (measureX << 8) | msg[1];
		measureZ = msg[2];
		measureZ = (measureX << 8) | msg[3];
		measureY = msg[4];
		measureY = (measureX << 8) | msg[5];

		Delay(34);
	}*/

	return 0;
}
