/*
 * Serial.h
 *
 *  Created on: 7 de abr de 2017
 *      Author: Otavio
 */

#ifndef DRIVERS_SERIAL_H_
#define DRIVERS_SERIAL_H_

#include "MKE02Z2.h"

typedef enum{
	Serial0,
	Serial1,
	Serial2
}SerialNumber;

typedef enum{
	None,
	Even,
	Odd
}Parity;

typedef enum{
	One,
	Two
}StopBits;

typedef void (*functionPtr)(void);

void Serial_Setup(SerialNumber serial, unsigned short baudrate, Parity parity, StopBits stopbits);
void Serial_Write(SerialNumber serial, unsigned char *data, unsigned short length);
void Serial_Read(SerialNumber serial, unsigned char *data, unsigned short length);

void Serial_EnableReceiveIRQ(SerialNumber serial);
void Serial_DisableReceiveIRQ(SerialNumber serial);

void Serial_SetIRQFunction(SerialNumber serial, functionPtr function);

#endif /* DRIVERS_SERIAL_H_ */
