/*
 * leds.c
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#include "leds.h"
#include "application.h"

static volatile uint8_t led_service_task;

void leds_init(void)
{
//	GPIO_SetBits(STATUS_LED_PORT, STATUS_LED);					// stavova LED on

	// zalozeni ulohy blikani zelene led
	led_service_task = Scheduler_Add_Task(LED_service, 0, LED_SLOW_SERVICE_PERIOD);
	if(led_service_task == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
	}
}

void leds_set_period(uint16_t PERIOD)
{
	// odstran stary task
	Scheduler_Delete_Task(led_service_task);

	// zaloz novy s novou periodou
	led_service_task = Scheduler_Add_Task(LED_service, 0, PERIOD);
	if(led_service_task == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
	}
}
