/*
 * leds.h
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#ifndef INCLUDES_LEDS_H_
#define INCLUDES_LEDS_H_

#include "stm32f0xx.h"
#include "scheduler.h"
#include "defs.h"
#include "services.h"

void leds_init(void);
void leds_set_period(uint16_t PERIOD);

#endif /* INCLUDES_LEDS_H_ */
