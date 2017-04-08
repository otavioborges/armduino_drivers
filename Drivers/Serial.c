/*
 * Serial.c
 *
 *  Created on: 7 de abr de 2017
 *      Author: Otavio
 */

#include "system_MKE02Z2.h"
#include "Serial.h"

functionPtr functionAdd;

void UART0_IRQHandler(void){
	functionAdd();
}

UART_Type* SelectUART(SerialNumber serial){
	switch(serial){
		case Serial0:
			return UART0;
		case Serial1:
			return UART1;
		case Serial2:
			return UART2;
		default:
			return UART0;
	}
}

void Serial_Setup(SerialNumber serial, unsigned short baudrate, Parity parity, StopBits stopbits){
	UART_Type* lUART = SelectUART(serial);

	uint32_t baudRateDivider = (((SystemCoreClock)>>4) + (baudrate>>1))/baudrate;

	// activate Clock
	switch(serial){
		case Serial0:
			SIM->SCGC |= SIM_SCGC_UART0_MASK;
			break;
		case Serial1:
			SIM->SCGC |= SIM_SCGC_UART1_MASK;
			break;
		case Serial2:
			SIM->SCGC |= SIM_SCGC_UART2_MASK;
			break;
	}

	lUART->BDH = (uint8_t)(baudRateDivider >> 8);
	if(stopbits == Two)
		lUART->BDH |= UART_BDH_SBNS_MASK;

	lUART->BDL = (uint8_t)(baudRateDivider & 0x00FF);
	lUART->C1 = UART_C1_UARTSWAI_MASK;

	switch(parity){
		case Even:
			lUART->C1 |= UART_C1_PE_MASK;
			lUART->C1 |= UART_C1_PT_MASK;
			break;
		case Odd:
			lUART->C1 |= UART_C1_PE_MASK;
			break;
	}

	lUART->C2 = (UART_C2_TE_MASK | UART_C2_RE_MASK);
	lUART->C3 = 0;
}

void Serial_Write(SerialNumber serial, unsigned char *data, unsigned short length){
	UART_Type* lUART = SelectUART(serial);

	while((lUART->S1 & UART_S1_TDRE_MASK) == 0);	// wait line to be available

	for(uint16_t pos = 0; pos < length; pos++){
		lUART->D = data[pos];
		while((lUART->S1 & UART_S1_TDRE_MASK) == 0);	// wait line to be available
	}
}

void Serial_Read(SerialNumber serial, unsigned char *data, unsigned short length){
	UART_Type* lUART = SelectUART(serial);

	uint16_t pos = 0;
	while(pos < length){
		if((lUART->S1 & UART_S1_RDRF_MASK)){
			data[pos] = lUART->D;
			pos++;
		}
	}
}

void Serial_EnableReceiveIRQ(SerialNumber serial){
	UART_Type* lUART = SelectUART(serial);

	lUART->C2 |= UART_C2_RIE_MASK;
	switch(serial){
		case Serial0:
			NVIC_EnableIRQ(UART0_IRQn);
			break;
		case Serial1:
			NVIC_EnableIRQ(UART1_IRQn);
			break;
		case Serial2:
			NVIC_EnableIRQ(UART2_IRQn);
			break;
	}
}
void Serial_DisableReceiveIRQ(SerialNumber serial){
	UART_Type* lUART = SelectUART(serial);

	lUART->C2 |= UART_C2_RIE_MASK;
	switch(serial){
		case Serial0:
			NVIC_DisableIRQ(UART0_IRQn);
			break;
		case Serial1:
			NVIC_DisableIRQ(UART1_IRQn);
			break;
		case Serial2:
			NVIC_DisableIRQ(UART2_IRQn);
			break;
	}
}

void Serial_SetIRQFunction(SerialNumber serial, functionPtr function){
	functionAdd = function;
}
