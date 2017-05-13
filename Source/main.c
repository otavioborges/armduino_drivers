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

//#include "NEC_IR.h"
//#include "Analog.h"
#include "TimedInt.h"
#include "Digital.h"
#include "Serial.h"

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

#define BUFFER_SIZE 2000u

uint8_t bufferIR[BUFFER_SIZE];
uint16_t bufferPos = 0;
uint8_t initiate = 0;
volatile uint8_t done = 0;

uint8_t countDown = 0;

void irRead(void){
	if((ACMP0->CS & ACMP_CS_ACO_MASK) && !done)
		initiate = 1;

	if(initiate){
		if(ACMP0->CS & ACMP_CS_ACO_MASK){
			bufferIR[bufferPos] = 0x01;
		}else{
			bufferIR[bufferPos] = 0;
		}

		bufferPos++;
		if(bufferPos >= BUFFER_SIZE){
			bufferPos = 0;
			initiate = 0;
			done = 1u;
			Digital_Write(DigitalLed,HIGH);
		}
	}
}

uint8_t findMatch(uint8_t *pattern){
	uint8_t variable = pattern[0] << 1;
	variable = (variable | pattern[1]) << 1;
	variable = (variable | pattern[2]) << 1;
	variable = (variable | pattern[3]) << 1;
	variable = (variable | pattern[4]);

	if((variable == 0b00000100) ||(variable == 0b00010000) ||(variable == 0b00001100))
		return 0x01;
	else if(variable == 0b00000000)
		return 0;
	else
		return 0x02;
}

int main(void){
	ConfigBoardDefaultMuxing();

	Digital_pinMode(DigitalLed,OUTPUT);

	SIM->SCGC |= SIM_SCGC_ACMP0_MASK;
	ACMP0->C0 = ACMP_C0_ACPSEL(0x03);
	ACMP0->C1 = (ACMP_C1_DACEN_MASK | ACMP_C1_DACREF_MASK | ACMP_C1_DACVAL(0x40));
	ACMP0->C2 = 0x01;
	ACMP0->CS = ACMP_CS_ACE_MASK;

	SIM->SOPT |= SIM_SOPT_ACIC_MASK;
	SIM->SCGC |= SIM_SCGC_FTM1_MASK;
	FTM1->SC = 0;
	//FTM1->CONF = 0xC0;
	//FTM1->FMS = 0;
	//FTM1->MODE = FTM_MODE_WPDIS_MASK;
	FTM1->CONTROLS[0].CnSC = FTM_CnSC_ELSA_MASK;
	FTM1->CONTROLS[0].CnV = 0x3FFF;
	//FTM1->FLTCTRL = FTM_FLTCTRL_FAULT0EN_MASK;
	//FTM1->FLTPOL = FTM_FLTPOL_FLT0POL_MASK;
	FTM1->SC = FTM_SC_CLKS(0x01);

	//TimedInt_SetIRQFunction(TimedChannel0,irRead);
	//TimedInt_Config(TimedChannel0,0.0000016447368421052631578947368421053);

	uint8_t data[200];
	uint16_t dataPos = 0;
	uint16_t readingPos = 0;
	uint8_t shift = 0;
	uint16_t mismatch = 0;
	uint16_t lastMismatch = 65535;

	uint8_t msg[2];

	while(1){
		if(FTM1->CONTROLS[0].CnV >= 0x3FFF){
			Digital_Write(DigitalLed,HIGH);
		}else{
			Digital_Write(DigitalLed,LOW);
		}

//		if(done){
//			//NVIC_DisableIRQ(PIT_CH0_IRQn);
//
//			dataPos = 0;
//			readingPos = 0;
//			mismatch = 0;
//
//			for(uint8_t i = 0; i < 200; i++)
//				data[i] = 0;
//
//			while((readingPos < 2000-4) && (dataPos < 200)){
//				if(bufferIR[readingPos] == 0x01){
//					switch(findMatch(bufferIR+readingPos)){
//						case 0:
//							shift++;
//							readingPos += 3;
//							break;
//						case 0x01:
//							data[dataPos] |= 0x01;
//							while(data[dataPos+1] == 1) dataPos++;
//							break;
//						case 0x02:
//							while(bufferIR[readingPos] == 0x01) readingPos++;
//							mismatch++;
//							break;
//					}
//
//					if(shift == 8){
//						dataPos++;
//						shift = 0;
//					}else{
//						data[dataPos] = data[dataPos] << 1;
//					}
//				}
//				readingPos++;
//			}
//
//			//NVIC_EnableIRQ(PIT_CH0_IRQn);
//			done = 0;
//			if(mismatch == 0){
//				__asm("nop");
//			}else{
//				PIT->CHANNEL[0].LDVAL--;
//			}
//
//			Digital_Write(DigitalLed,LOW);
//
//			for(uint16_t i = 0; i < 2000; i++)
//				bufferIR[i] = 0;
//		}
	}

	return 0;
}
