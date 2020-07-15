/*
 * commands.c
 *
 *  Created on: 7. 5. 2019
 *      Author: Petr Dvorak
 */

#include "commands.h"
#include "string.h"
#include "application.h"
#include "bootloader.h"
#include "errorcodes.h"

// prikazy
static const uint8_t COMMAND_BLANK[] = "";
static command_t command_blank = {(uint8_t *)&COMMAND_BLANK, NULL, TRUE};				// prazdny prikaz, pouze enter

// get version
static const uint8_t COMMAND_BLGETVER[] = "GETV****";
static command_t command_getver = {(uint8_t *)&COMMAND_BLGETVER, &cmd_getver, TRUE};	// prikaz GETV

// firmware update request
static const uint8_t COMMAND_FUREQ[] = "FURQ****";
static command_t command_fureq = {(uint8_t *)&COMMAND_FUREQ, &cmd_fureq, TRUE};			// prikaz FURQ

// memory clear password request
static const uint8_t COMMAND_MCPR[] = "MCPR**********";
static command_t command_mcpr = {(uint8_t *)&COMMAND_MCPR, &cmd_mcpr, TRUE};			// prikaz MCPR******xxxx

// memory lock request
static const uint8_t COMMAND_MLREQ[] = "MLRQ****";
static command_t command_mlreq = {(uint8_t *)&COMMAND_MLREQ, &cmd_mlreq, TRUE};			// prikaz MLRQ

// firmware update stop
static const uint8_t COMMAND_FUSTOP[] = "STOP****";
static command_t command_fustop = {(uint8_t *)&COMMAND_FUSTOP, &cmd_fustop, TRUE};		// prikaz STOP

// final CRC check
static const uint8_t COMMAND_FICRC[] = "FCRC********";
static command_t command_ficrc = {(uint8_t *)&COMMAND_FICRC, &cmd_ficrc, TRUE};			// prikaz FICRC

// bootloader restart
static const uint8_t COMMAND_REST[] = "REST****";
static command_t command_rest = {(uint8_t *)&COMMAND_REST, &cmd_rest, TRUE};			// prikaz REST

// chunk transfer request
static const uint8_t COMMAND_CHNN[] = "CH******";
static command_t command_chnn = {(uint8_t *)&COMMAND_CHNN, &cmd_chnn, TRUE};			// prikaz CHNN




COMMAND_STATUS commands_parse(uint8_t *pattern, uint8_t *command);
uint8_t cmd_isnumber(uint8_t *character);

void commands_process(void)
{
	COMMAND_STATUS (*p_func)();

#ifdef CRCCMDPROTECTION
	uint32_t *p_crc = (uint32_t *)get_Rx_buffer;
	if (bootloader_crc_cmd_check(p_crc[1]) == RETURN_ERROR)
	{
		return;
	}
#endif // CRCCMDPROTECTION

	// parsing prikazu GETV
	if (commands_parse((uint8_t *)&COMMAND_BLGETVER, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_getver.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_getver.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu FURQ
	else if (commands_parse((uint8_t *)&COMMAND_FUREQ, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_fureq.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_fureq.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu MCPR
	else if (commands_parse((uint8_t *)&COMMAND_MCPR, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_mcpr.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_mcpr.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu MLRQ
	else if (commands_parse((uint8_t *)&COMMAND_MLREQ, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_mlreq.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_mlreq.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu STOP
	else if (commands_parse((uint8_t *)&COMMAND_FUSTOP, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_fustop.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_fustop.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu FCRC
	else if (commands_parse((uint8_t *)&COMMAND_FICRC, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_ficrc.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_ficrc.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu REST
	else if (commands_parse((uint8_t *)&COMMAND_REST, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_rest.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_rest.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prikazu CHNN
	else if (commands_parse((uint8_t *)&COMMAND_CHNN, get_Rx_buffer()) == COMMANDOK)
	{
		if (command_chnn.enabled)
		{
			// zavolani obsluzne funkce prikazu
			p_func = command_chnn.p_itemfunc;
			p_func(get_Rx_buffer());
		}
		return;
	}
	// parsing prazdneho prikazu
	else if (commands_parse(command_blank.command, get_Rx_buffer()) == COMMANDBLANK)
	{
		if (command_blank.enabled)
		{
			// zavolani obsluzne funkce prikazu
			commands_ok_cmd();
		}
		return;
	}
	else
	{
		// zbyva jiz jen posledni varianta - spatny prikaz
		commands_wrong_cmd();
	}

}



COMMAND_STATUS commands_parse(uint8_t *pattern, uint8_t *command)
{
	volatile uint8_t i = 0;
	volatile uint8_t retval = COMMANDWRONG;

	// test kvuli odstraneni testu prazdneho prikazu (enteru)
	if (command[0] != 0)
	{
		// prikaz neni prazdny
		//
		// test na odlisnou delkum napr. HBD a HBDN, pokud maji retezce odlisnou desku, rozhodne se lisi
		if (strlen((const char *)pattern) != strlen((const char *)command))
		{
			retval = COMMANDWRONG;
			return retval;
		}
		// retezce maji stejnou delku
		//
		// porovnani prichoziho prikazu se vzorem. Hvezdicka je zastupny znak pro jedinou cifru cisla.
		// projdi vsechny znaky vzoru, maximum je delka bufferu, pokud by se cokoliv stalo pri testu delek vyse
		while ((pattern[i] != 0) && (i < RXBUFFERSIZE))
		{
			if (pattern[i] == command[i])
			{
				// shoduji se
				//
				// pokracuj
				retval = COMMANDOK;
				i++;
				continue;
			}
			else
			{
				// jsou ruzne
				//
				// test na hvezdicku
				if (pattern[i] == '*')
				{
					retval = COMMANDOK;
					i++;
					continue;
				}
				else
				{
					// odlisuji se a znak neni hvezdicka, tedy se retezce lisi
					retval = COMMANDWRONG;
					return retval;
				}
			}
		}
	}
	else
	{
		// prazdny prikaz
		retval = COMMANDBLANK;
	}
	return retval;
}

void commands_wrong_cmd(void)
{
	uint8_t text[] = "ER";

	usart_send_text((uint8_t *)&text);
	usart_newline();
}

void commands_ok_cmd(void)
{
	uint8_t text[] = "OK";

	usart_send_text((uint8_t *)&text);
	usart_newline();
}

// handlery prikazu

// prikaz GETVER
COMMAND_STATUS cmd_getver(void *p_i)
{
	if (bootloader_getversion(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

// prikaz FUREQ
COMMAND_STATUS cmd_fureq(void *p_i)
{
	// zastav spousteni aplikace
	if (bootloader_fureq(p_i) == RETURN_ERROR)
	{
		return COMMANDWRONG;
	}
	return COMMANDOK;
}

// prikaz MCPR
COMMAND_STATUS cmd_mcpr(void *p_i)
{
	if (bootloader_mem_clear(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

// prikaz MLREQ
COMMAND_STATUS cmd_mlreq(void *p_i)
{
	if (bootloader_mem_lock(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

// prikaz FUSTOP
COMMAND_STATUS cmd_fustop(void *p_i)
{
	if (bootloader_update_stop(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

// prikaz FICRC
COMMAND_STATUS cmd_ficrc(void *p_i)
{
	if (bootloader_final_check(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

// prikaz REST
COMMAND_STATUS cmd_rest(void *p_i)
{
	if (bootloader_restart(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

// prikaz CHNN
COMMAND_STATUS cmd_chnn(void *p_i)
{
	if (bootloader_chunk_request(p_i) == RETURN_NORMAL)
	{
		return COMMANDOK;
	}
	else
	{
		return COMMANDWRONG;
	}
}

uint8_t cmd_isnumber(uint8_t *character)
{
	if ((*character >= '0') && (*character <= '9'))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
