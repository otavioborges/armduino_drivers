/*
 * NEC_IR.h
 *
 *  Created on: 10/05/2017
 *      Author: Otavio
 */

#ifndef LIBRARIES_NEC_IR_H_
#define LIBRARIES_NEC_IR_H_

#define NEC_IR_MAX_COMMANDS 10u

#include "MKE02Z2.h"
#include "Analog.h"
#include "TimedInt.h"

void NEC_IR_Init(PWM ir_pin, TimedChannel channel);
uint8_t NEC_IR_ItensOnBuffer(void);
void NEC_IR_SendCommand(uint8_t manufacturer, uint8_t command, uint8_t value);

#endif /* LIBRARIES_NEC_IR_H_ */
