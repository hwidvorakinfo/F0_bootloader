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

COMMAND_STATUS cmd_getver(void *p_i);
COMMAND_STATUS cmd_fureq(void *p_i);
COMMAND_STATUS cmd_mcpr(void *p_i);
COMMAND_STATUS cmd_mlreq(void *p_i);
COMMAND_STATUS cmd_fustop(void *p_i);
COMMAND_STATUS cmd_ficrc(void *p_i);
COMMAND_STATUS cmd_rest(void *p_i);
COMMAND_STATUS cmd_chnn(void *p_i);

#endif /* COMMANDS_H_ */
