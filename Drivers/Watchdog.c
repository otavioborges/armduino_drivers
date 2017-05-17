/*
 * Watchdog.c
 *
 *  Created on: 17/05/2017
 *      Author: Otavio
 */

#define WDOG_UNLOCK	WDOG->CNT = 0x20C5;		\
					WDOG->CNT = 0x28D9;

#include "Watchdog.h"

functionPtr functionWatchdog;

void Watchdog_IRQHandler(void){
	functionWatchdog();
}

uint8_t Watchdog_Setup(float timeout, uint8_t enableInterrupt){

	uint32_t timerValue = (SystemCoreClock * timeout);
	if(timerValue > 0xFFFF)
		return 0;
	else if(timerValue == 0)
		return 0;

	__disable_irq();
	WDOG_UNLOCK
	WDOG->TOVAL8B.TOVALL = timerValue;
	WDOG->TOVAL8B.TOVALH = (timerValue >> 8);

	WDOG->CS2 = 0;
	if(enableInterrupt)
		WDOG->CS1 |= (WDOG_CS1_EN_MASK | WDOG_CS1_INT_MASK);
	else
		WDOG->CS1 |= WDOG_CS1_EN_MASK;
	__enable_irq();

	return 0x01;
}

void Watchdog_Disable(void){
	__disable_irq();
	WDOG_UNLOCK
	WDOG->CS1 &= ~(WDOG_CS1_EN_MASK | WDOG_CS1_INT_MASK);
	__enable_irq();
}

void Watchdog_Reset(void){
	__disable_irq();
	WDOG->CNT = 0x02A6;
	WDOG->CNT = 0x80B4;
	__enable_irq();
}

void Watchdog_SetIRQFunction(functionPtr function){
	functionWatchdog = function;
}
