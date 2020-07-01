/*
 * services.h
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#ifndef INCLUDES_SERVICES_H_
#define INCLUDES_SERVICES_H_

#include "stm32f0xx.h"
#include "delay.h"
#include "leds.h"
#include "scheduler.h"
#include "usart.h"
#include "commands.h"

#define MILISEKUND 	/1000

// periody jsou v milisekundach, neboli zakladni periode SysTick casovace
#define LED_SLOW_SERVICE_PERIOD			(SCHEDULERPERIOD * 2000 MILISEKUND)
#define LED_FAST_SERVICE_PERIOD			(SCHEDULERPERIOD * 1000 MILISEKUND)
#define LED_ALARM_SERVICE_PERIOD		(SCHEDULERPERIOD * 250 MILISEKUND)
#define REGULATORS_SERVICE_PERIOD		(SCHEDULERPERIOD * 10 MILISEKUND)
#define MONITORING_SERVICE_PERIOD		(SCHEDULERPERIOD * 2000 MILISEKUND)
#define ANIMATION_SERVICE_PERIOD		(SCHEDULERPERIOD * 125 MILISEKUND)
#define HEARTBEAT_SERVICE_PERIOD		(SCHEDULERPERIOD * 1000 MILISEKUND)
#define APPSTART_SERVICE_PERIOD			(SCHEDULERPERIOD * 2000 MILISEKUND)

#define FREE			0
#define STARTED			127
#define REQUESTED		64


// sluzby
void LED_service(void);
void Delay_service(void);
void Command_service(void);
void Heartbeat_service(void);
void AppStart_service(void);

#endif /* INCLUDES_SERVICES_H_ */
