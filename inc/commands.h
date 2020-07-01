/*
 * commands.h
 *
 *  Created on: 7. 5. 2019
 *      Author: Petr Dvorak
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <regulator_types.h>
#include "stm32f0xx.h"
#include "defs.h"
#include "usart.h"

typedef enum {
	COMMANDOK = 0,
	COMMANDWRONG,
	COMMANDBLANK
} COMMAND_STATUS;

typedef struct {
	uint8_t *command;
	COMMAND_STATUS (*p_itemfunc)(void *);
	uint8_t enabled;
} command_t;

void commands_process(void);
void commands_wrong_cmd(void);
void commands_ok_cmd(void);

COMMAND_STATUS cmd_heatbeddisable(void *p_i);
COMMAND_STATUS cmd_heatbedenable(void *p_i);
COMMAND_STATUS cmd_heatbedgetstatus(void *p_i);
COMMAND_STATUS cmd_heatbedget1temp(void *p_i);
COMMAND_STATUS cmd_heatbedgetall(void *p_i);
COMMAND_STATUS cmd_heatbedset1temp(void *p_i);
COMMAND_STATUS cmd_heatbedsetall(void *p_i);
COMMAND_STATUS cmd_heatbedsetgloball(void *p_i);
COMMAND_STATUS cmd_heatbedconst(void *p_i);
COMMAND_STATUS cmd_heatbedload(void *p_i);
COMMAND_STATUS cmd_heatbedversion(void *p_i);

#endif /* COMMANDS_H_ */
