/*
 * NEC_IR.c
 *
 *  Created on: 10/05/2017
 *      Author: Otavio
 */

#define ADDR_8BIT 0x00
#define ADDR_16BIT 0x01

#define IDLE		0x00
#define BIT0_BURST	0x01
#define BIT0_SPACE0	0x02
#define BIT1_BURST	0x03
#define BIT1_SPACE0	0x04
#define BIT1_SPACE1 0x05
#define BIT1_SPACE2 0x06

#define SEND_BURST		0x00
#define SEND_ADDR8		0x01
#define SEND_INV_ADDR8	0x02
#define SEND_ADDR16		0x03
#define SEND_COMM		0x04
#define SEND_INV_COMM	0x05

#define BURST_FULL		0x01A2
#define BURST_HALF		0x00D1
#define BURST_EMPTY		0x0000

#include "NEC_IR.h"
#include "TimedInt.h"

FTM_Type *ftm_periph;
PWM pin_used;

typedef struct {
	uint8_t state;
	uint8_t agcState;
	uint8_t sending;
	uint8_t addrLength;
	uint16_t address;
	uint8_t invAddress;
	uint8_t command;
	uint8_t invCommand;
	uint8_t currentBit;
} NECCommand;

uint8_t itensOnBuffer = 0;
uint8_t writePos = 0;
uint8_t readPos = 0;
NECCommand buffer[10];

volatile uint8_t transmitting = 0;

void Send_data(NECCommand *data){
	if(data->state == IDLE){
		Analog_Write(pin_used, BURST_HALF);

		switch(data->sending){
			case SEND_ADDR8:
			case SEND_ADDR16:
				if(data->address & 0x0001)
					data->state = BIT1_BURST;
				else
					data->state = BIT0_BURST;

				data->address = data->address >> 1;
				break;
			case SEND_COMM:
				if(data->command & 0x01)
					data->state = BIT1_BURST;
				else
					data->state = BIT0_BURST;

				data->command = data->command >> 1;
				break;
			case SEND_INV_ADDR8:
				if(data->invAddress & 0x01)
					data->state = BIT1_BURST;
				else
					data->state = BIT0_BURST;

				data->invAddress = data->invAddress >> 1;
				break;
			case SEND_INV_COMM:
				if(data->invCommand & 0x01)
					data->state = BIT1_BURST;
				else
					data->state = BIT0_BURST;

				data->invCommand = data->invCommand >> 1;
				break;
		}
	}else if(data->state == BIT1_BURST){
		Analog_Write(pin_used,BURST_EMPTY);
		data->state = BIT1_SPACE0;
	}else if(data->state == BIT1_SPACE0){
		data->state = BIT1_SPACE1;
	}else if(data->state == BIT1_SPACE1){
		data->state = BIT1_SPACE2;
	}else if(data->state == BIT1_SPACE2){
		data->state = IDLE;
	}else if(data->state == BIT0_BURST){
		Analog_Write(pin_used,BURST_EMPTY);
		data->state = BIT0_SPACE0;
	}else if(data->state == BIT0_SPACE0){
		data->state = IDLE;
	}
}

void PIT_Callback(void){
	if(itensOnBuffer == 0) // nothing to send
		return;

	transmitting = 0x01; // block buffer

	NECCommand *toSend = &buffer[readPos];

	if(toSend->sending == SEND_BURST){
		if(toSend->agcState == 0){
			Analog_Write(pin_used,BURST_FULL);
		}else if(toSend->agcState == 16u){
			Analog_Write(pin_used,BURST_EMPTY);
		}else if(toSend->agcState == 23u){
			if(toSend->addrLength == ADDR_8BIT)
				toSend->sending = SEND_ADDR8;
			else
				toSend->sending = SEND_ADDR16;
		}

		toSend->agcState++;
	}else if(toSend->sending == SEND_ADDR8){
		Send_data(toSend);
		if(toSend->state == IDLE){
			toSend->currentBit++;
			if(toSend->currentBit == 8){
				toSend->currentBit = 0;
				toSend->sending = SEND_INV_ADDR8;
			}
		}
	}else if(toSend->sending == SEND_ADDR16){
		Send_data(toSend);
		if(toSend->state == IDLE){
			toSend->currentBit++;
			if(toSend->currentBit == 16){
				toSend->currentBit = 0;
				toSend->sending = SEND_COMM;
			}
		}
	}else if(toSend->sending == SEND_INV_ADDR8){
		Send_data(toSend);
		if(toSend->state == IDLE){
			toSend->currentBit++;
			if(toSend->currentBit == 8){
				toSend->currentBit = 0;
				toSend->sending = SEND_COMM;
			}
		}
	}else if(toSend->sending == SEND_COMM){
		Send_data(toSend);
		if(toSend->state == IDLE){
			toSend->currentBit++;
			if(toSend->currentBit == 8){
				toSend->currentBit = 0;
				toSend->sending = SEND_INV_COMM;
			}
		}
	}else if(toSend->sending == SEND_INV_COMM){
		Send_data(toSend);
		if(toSend->state == IDLE){
			toSend->currentBit++;
			if(toSend->currentBit == 8){
				toSend->currentBit = 0;

				// reach next info to send
				readPos++;
				if(readPos == 10)
					readPos = 0;
				itensOnBuffer--;
			}
		}
	}

	transmitting = 0x00; // unlock buffer
}

void NEC_IR_Init(PWM ir_pin, TimedChannel channel){
	Analog_InitPWM();

	FTM0->MOD = BURST_FULL;
	FTM2->MOD = BURST_FULL;

	Analog_SetPWMPin(ir_pin,0);

	pin_used = ir_pin;
	// config PIT to coordinate bursts
	TimedInt_SetIRQFunction(channel,PIT_Callback);

	if((SIM->SCGC & SIM_SCGC_PIT_MASK) == 0){
		SIM->SCGC |= SIM_SCGC_PIT_MASK;
	}

	PIT->MCR = PIT_MCR_MDIS_MASK; // disable clocks for config
	// ideal freq is 1,7...kHz (562.5us)
	PIT->CHANNEL[(uint8_t)channel].LDVAL = (uint32_t)(SystemCoreClock * 0.0005625);
	PIT->CHANNEL[(uint8_t)channel].TCTRL = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);

	if(channel == TimedChannel0)
		NVIC_EnableIRQ(PIT_CH0_IRQn);
	else
		NVIC_EnableIRQ(PIT_CH1_IRQn);

	PIT->MCR = PIT_MCR_FRZ_MASK; // enable clocks
}

uint8_t NEC_IR_ItensOnBuffer(void){
	return itensOnBuffer;
}

void NEC_IR_SendCommand(uint16_t addr, uint8_t command, uint8_t addr16bit){
	if(itensOnBuffer >= 10) // buffer full
		return;

	while(transmitting)	// wait transmitting end
		__asm("nop");

	if(addr16bit){
		buffer[writePos].state = IDLE;
		buffer[writePos].agcState = 0;
		buffer[writePos].sending = SEND_BURST;
		buffer[writePos].addrLength = ADDR_16BIT;
		buffer[writePos].address = addr;
		buffer[writePos].command = command;
		buffer[writePos].invCommand = ~(command);
		buffer[writePos].currentBit = 0;
	}else{
		buffer[writePos].state = IDLE;
		buffer[writePos].agcState = 0;
		buffer[writePos].sending = SEND_BURST;
		buffer[writePos].addrLength = ADDR_8BIT;
		buffer[writePos].address = (uint8_t)addr;
		buffer[writePos].invAddress = (uint8_t)(~(addr));
		buffer[writePos].command = command;
		buffer[writePos].invCommand = ~(command);
		buffer[writePos].currentBit = 0;
	}
	writePos++;
	if(writePos == 10)
		writePos = 0;

	itensOnBuffer++;
}
