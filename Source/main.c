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

#include "Digital.h"

int main(void){
	// Configure board pins and auxiliary functions
	ConfigBoardDefaultMuxing();

	// Set board led as output
	Digital_pinMode(DigitalLed, OUTPUT);

	while(1){
		// Toggle the state of the led
		Digital_Toggle(DigitalLed);
		// wait 1s
		Delay(1000u);
	}

	return 0;
}
