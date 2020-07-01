/*
 * bootloader.c
 *
 *  Created on: 1. 7. 2020
 *      Author: Petr Dvorak
 */

#include "bootloader.h"
#include "services.h"
#include "scheduler.h"

static uint8_t appstart_task;

void bootloader_init(void)
{
	// zalozeni ulohy startu aplikace
	appstart_task = Scheduler_Add_Task(AppStart_service, APPSTART_SERVICE_PERIOD, 0);
	if(appstart_task == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
	}
}

