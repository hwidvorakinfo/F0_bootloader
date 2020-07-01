/*
 * commands.c
 *
 *  Created on: 7. 5. 2019
 *      Author: Petr Dvorak
 */

#include "commands.h"
#include "string.h"
#include "application.h"

// prikazy
static const uint8_t COMMAND_BLANK[] = "";
static command_t command_blank = {(uint8_t *)&COMMAND_BLANK, NULL, TRUE};				// prazdny prikaz, pouze enter
static const uint8_t COMMAND_HBD[] = "HBD";
static command_t command_hbd = {(uint8_t *)&COMMAND_HBD, &cmd_heatbeddisable, TRUE};	// prikaz HBD, heatbed disable
static const uint8_t COMMAND_HBE[] = "HBE";
static command_t command_hbe = {(uint8_t *)&COMMAND_HBE, &cmd_heatbedenable, TRUE};		// prikaz HBE, heatbed enable
static const uint8_t COMMAND_HBGS[] = "HBGS";
static command_t command_hbgs = {(uint8_t *)&COMMAND_HBGS, &cmd_heatbedgetstatus, TRUE};// prikaz HBGS, heatbed get status
static const uint8_t COMMAND_HBG1T[] = "HBG1T**";
static command_t command_hbg1t = {(uint8_t *)&COMMAND_HBG1T, &cmd_heatbedget1temp, TRUE}; // prikaz HBG1Txx, get temperature one bed xx
static const uint8_t COMMAND_HBGAT[] = "HBGAT";
static command_t command_hbgat = {(uint8_t *)&COMMAND_HBGAT, &cmd_heatbedgetall, TRUE}; 	// prikaz HBGAT, get all temperatures
static const uint8_t COMMAND_HBS1T[] = "HBS1T**_***";
static command_t command_hbs1t = {(uint8_t *)&COMMAND_HBS1T, &cmd_heatbedset1temp, TRUE}; // prikaz HBS1Txx, set temperature one bed xx
static const uint8_t COMMAND_HBSAT[] = "HBSAT**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***_**_***";
static command_t command_hbsat = {(uint8_t *)&COMMAND_HBSAT, &cmd_heatbedsetall, TRUE}; // prikaz HBSATxx_TTT_xx_TTT_xx_TTT, set all temperatures
static const uint8_t COMMAND_HBSGT[] = "HBSGT***";
static command_t command_hbsgt = {(uint8_t *)&COMMAND_HBSGT, &cmd_heatbedsetgloball, TRUE}; // prikaz HBSGTttt, set all temperatures globally
static const uint8_t COMMAND_HBC[] = "HBC******.*";
static volatile command_t command_hbc = {(uint8_t *)&COMMAND_HBC, &cmd_heatbedconst, TRUE};	// prikaz HBCNNTS**.*, nastav danou konstantu zesileni, NN je cislo bedu, T je typ konstanty, S je znamenko a **.* je cislo v desetinnem tvaru
static const uint8_t COMMAND_HBML[] = "HBML**";
static volatile command_t command_hbml = {(uint8_t *)&COMMAND_HBML, &cmd_heatbedload, TRUE};	// prikaz HBML**, nastav maximalni zatez, ** je hodnota, od 00 do HBL_MODULAR_BED_COUNT
static const uint8_t COMMAND_HBV[] = "HBV";
static volatile command_t command_hbv = {(uint8_t *)&COMMAND_HBV, &cmd_heatbedversion, TRUE};	// prikaz HBV, ziskej cislo verze desky



COMMAND_STATUS commands_parse(uint8_t *pattern, uint8_t *command);
uint8_t cmd_isnumber(uint8_t *character);

void commands_process(void)
{
	//if (heatbed_ready() == TRUE)
	{
		COMMAND_STATUS (*p_func)();

		// parsing prikazu HBD
		if (commands_parse((uint8_t *)&COMMAND_HBD, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbd.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbd.p_itemfunc;
				p_func(NULL);
			}
			return;
		}
		// parsing prikazu HBE
		else if (commands_parse((uint8_t *)&COMMAND_HBE, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbe.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbe.p_itemfunc;
				p_func(NULL);
			}
			return;
		}
		// parsing prikazu HBGS
		else if (commands_parse((uint8_t *)&COMMAND_HBGS, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbgs.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbgs.p_itemfunc;
				p_func(NULL);
			}
			return;
		}
		// parsing prikazu HBG1T
		else if (commands_parse((uint8_t *)&COMMAND_HBG1T, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbg1t.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbg1t.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBGAT
		else if (commands_parse((uint8_t *)&COMMAND_HBGAT, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbgat.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbgat.p_itemfunc;
				p_func(NULL);
			}
			return;
		}
		// parsing prikazu HBS1T
		else if (commands_parse((uint8_t *)&COMMAND_HBS1T, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbs1t.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbs1t.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBSAT
		else if (commands_parse((uint8_t *)&COMMAND_HBSAT, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbsat.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbsat.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBSGT
		else if (commands_parse((uint8_t *)&COMMAND_HBSGT, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbsgt.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbsgt.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBC****.*
		else if (commands_parse(command_hbc.command, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbc.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbc.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBML**
		else if (commands_parse(command_hbml.command, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbml.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbml.p_itemfunc;
				p_func(get_Rx_buffer());
			}
			return;
		}
		// parsing prikazu HBV
		else if (commands_parse(command_hbv.command, get_Rx_buffer()) == COMMANDOK)
		{
			if (command_hbv.enabled)
			{
				// zavolani obsluzne funkce prikazu
				p_func = command_hbv.p_itemfunc;
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

// prikaz HBD
COMMAND_STATUS cmd_heatbeddisable(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBE
COMMAND_STATUS cmd_heatbedenable(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBGS
COMMAND_STATUS cmd_heatbedgetstatus(void *p_i)
{
	uint8_t text[] = "DISABLED";
	hbl_heatbed_t *p_bed;

	//p_bed = hbl_get_modular_bed();

	if (p_bed->status == HBLENABLED)
	{
		strcpy((char *)&text, (char *)&"ENABLED");
	}
	else
	if (p_bed->status == HBLDISABLED)
	{
		strcpy((char *)&text, (char *)&"DISABLED");
	}
	usart_send_text((uint8_t *)&text);
	usart_newline();

	return COMMANDOK;
}

// prikaz HBG1Txx
COMMAND_STATUS cmd_heatbedget1temp(void *p_i)
{
	return COMMANDOK;
}


// prikaz HBGAT
COMMAND_STATUS cmd_heatbedgetall(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBS1T
COMMAND_STATUS cmd_heatbedset1temp(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBSAT
COMMAND_STATUS cmd_heatbedsetall(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBSGT
COMMAND_STATUS cmd_heatbedsetgloball(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBCxx****.* - HBC00P+05.0
COMMAND_STATUS cmd_heatbedconst(void *p_i)
{
	return COMMANDOK;
}

// prikaz HBMLxx
COMMAND_STATUS cmd_heatbedload(void *p_i)
{
	return COMMANDOK;
}


// prikaz HBV
COMMAND_STATUS cmd_heatbedversion(void *p_i)
{
	uint8_t text[] = "MHB vx.x";
	uint8_t version;

#define MAJORNUM	5
#define MINORNUM	7

	//version = hbl_get_version();

	switch (version)
	{
		case HWVERSION_V01:
			text[MAJORNUM] = '0';
			text[MINORNUM] = '1';
		break;

		case HWVERSION_V02:
			text[MAJORNUM] = '0';
			text[MINORNUM] = '2';
		break;

		default:
			text[MAJORNUM] = 'N';
			text[MINORNUM] = 'M';
		break;
	}

	usart_send_text((uint8_t *)&text);
	usart_newline();

	return COMMANDOK;

#undef MAJORNUM
#undef MINORNUM
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
