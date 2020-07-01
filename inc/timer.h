/*
 * timer.h
 *
 *  Created on: 14. 8. 2019
 *      Author: Petr Dvorak
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f0xx.h"
#include "defs.h"
#include "stm32f0xx_conf.h"

#define APB1CLK			42000000
#define APB2CLK			84000000

void timer_capture_init(void);
void timer_timebase_init(void);

#endif /* TIMER_H_ */
