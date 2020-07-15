/*
 * bootloader.c
 *
 *  Created on: 1. 7. 2020
 *      Author: Petr Dvorak
 */

#include "bootloader.h"
#include "services.h"
#include "scheduler.h"
#include "application.h"
#include "string.h"
#include "errorcodes.h"

static uint8_t appstart_task;			// cislo tasku automatickeho spoustece aplikace
static AUTOMAT_STATE fu_state;			// ridici promenna stavoveho automatu
static uint8_t password[] = "w1peMe";	// heslo k prikazu vymazani pameti
static uint16_t chunk_len = 0;			// ocekavana delka dat po chunk requestu
static uint32_t flash_address = APPLICATION_ADDRESS;	// adresa zapisu do flash

extern usart_data_rx_t Rx;

void bootloader_sendreply(uint8_t *text);
void bootloader_crc_config(void);
void bootloader_crc_fail(void);

uint8_t bootloader_init(void)
{
	uint8_t retval = RETURN_NORMAL;

	// inicializace
	bootloader_crc_config();

//	// zalozeni ulohy startu aplikace
//	appstart_task = Scheduler_Add_Task(AppStart_service, APPSTART_SERVICE_PERIOD, 0);
//	if(appstart_task == SCH_MAX_TASKS)
//	{
//		// chyba pri zalozeni service
//		retval = RETURN_ERROR;
//	}
//	else
//	{
//		retval = RETURN_NORMAL;
//	}
	return retval;
}

void bootloader_set_state(AUTOMAT_STATE newstate)
{
	fu_state = newstate;
}

AUTOMAT_STATE bootloader_get_state(void)
{
	return fu_state;
}

void bootloader_sendreply(uint8_t *text)
{
	usart_send_text((uint8_t *)text);
	usart_newline();
}

//
// vraci cislo verze
CMD_RETURN bootloader_getversion(void *p_i)
{
	uint8_t text[] = "VERx.xx";

//#ifdef CRCCMDPROTECTION
//	uint32_t *p_crc = p_i;
//	if (bootloader_crc_cmd_check(p_crc) == RETURN_ERROR)
//	{
//		return RETURN_ERROR;
//	}
//#endif // CRCCMDPROTECTION

	strcpy((char *)&text[3], (char *)BLVERSION);
	bootloader_sendreply(text);

	return RETURN_NORMAL;
}

//
// provadi vymazani pameti
CMD_RETURN bootloader_mem_clear(void *p_i)
{
	// mazat pamet je mozne ve stavech:
	//	FIRMUPDATE_CLEAR_REQ,
	//	FIRMUPDATE_CLEAR_FAILED,
	//	FIRMUPDATE_PROCESS,
	//	FIRMUPDATE_FINAL,
	//	FIRMUPDATE_LOCKED,
	//	FIRMUPDATE_ERROR

	if (fu_state > FIRMUPDATE_REQUESTED)
	{
		// pozadavek ve spatnem stavu - zaporna odpoved bez akce
		bootloader_memclear_fail();
		return RETURN_ERROR;
	}

	// porovnani hesla na vstupu s ulozenym heslem
	uint8_t *p_char = p_i;
	if (bootloader_password_check(&p_char[4]) == RETURN_NORMAL)
	{
		// hesla se shoduji - vymaz pamet a posun se do dalsiho kroku stavoveho automatu
		bootloader_set_state(FIRMUPDATE_CLEAR_REQ);

		if (bootloader_memclear_do() == RETURN_NORMAL)
		{
			// vymazana cela pamet pro aplikaci, update proces muze zacit
			bootloader_set_state(FIRMUPDATE_PROCESS);
		}
		else
		{
			// chyba pri mazani
			bootloader_set_state(FIRMUPDATE_CLEAR_FAILED);
		}
	}
	else
	{
		// hesla se neshoduji - vrat zapornou odpoved, zustan v tomto stavu automatu
		bootloader_memclear_fail();
		return RETURN_ERROR;
	}

	return RETURN_NORMAL;
}

//
// inicializuje firmware update
CMD_RETURN bootloader_fureq(void *p_i)
{
	uint8_t text[] = "FURQ_OK";

	// vymaz sluzbu appstart
	if (Scheduler_Delete_Task(appstart_task) != RETURN_NORMAL)
	{
		// uloha neexistovala
	}

	// nastav stav stavoveho automatu
	fu_state = FIRMUPDATE_REQUESTED;

	flash_address = APPLICATION_ADDRESS;			// zacneme od zacatku, pripadne reinicializace opet na zacatek pri neuspesnem zapisu pred chvili

	bootloader_sendreply(text);

	return RETURN_NORMAL;
}

//
// zamyka pamet
CMD_RETURN bootloader_mem_lock(void *p_i)
{
	if (fu_state != FIRMUPDATE_FINAL)
	{
		// pozadavek ve spatnem stavu - zaporna odpoved bez akce
		bootloader_memlock_fail();
		return RETURN_ERROR;
	}
	// zamkni pamet aplikace
	bootloader_mlreq();

	return RETURN_NORMAL;
}

//
// ukoncuje firmware update proces
CMD_RETURN bootloader_update_stop(void *p_i)
{
	// vratit se do stavu IDLE je mozne z kterehokoliv stavu, pokud je pamet v poradku

	// zkontroluj celou pamet aplikace, pokud sedi CRC32, prepni do FIRMUPDATE_IDLE
	uint32_t *appcrc = (uint32_t *)APPLICATION_SPACE_CRC;

	if (bootloader_crc_check_block(APPLICATION_ADDRESS, APPLICATION_SPACE/sizeof(uint32_t), *appcrc) == RETURN_NORMAL)
	{
		fu_state = FIRMUPDATE_IDLE;
		FLASH_Lock();
		bootloader_send_message((uint8_t *)"STOP_OK");
	}
	else
	{
		// nesedi CRC32 cele pameti
		bootloader_send_message((uint8_t *)"STOP_NO");
	}

	return RETURN_NORMAL;
}

//
// provadi test pameti
CMD_RETURN bootloader_final_check(void *p_i)
{
	if (fu_state != FIRMUPDATE_PROCESS)
	{
		// pozadavek ve spatnem stavu - zaporna odpoved bez akce
		bootloader_send_message((uint8_t *)"FCRC_NO");
		return RETURN_ERROR;
	}

	// preneseny CRC
	uint8_t *p_byte = p_i;
	uint32_t crcref;
	crcref = (p_byte[4] << 24) + (p_byte[5] << 16) + (p_byte[6] << 8) + p_byte[7];

	// vypocet CRC cele pameti a ulozeni do stavove stranky
	uint32_t crc;

	CRC_SetInitRegister(0);
	crc = CRC_CalcBlockCRC((uint32_t *)APPLICATION_ADDRESS, APPLICATION_SPACE/4);

	if (crcref == crc)
	{
		if (FLASH_ErasePage(APPDATA_ADDRESS) == FLASH_COMPLETE)
		{
			// korektni vymazani stavove stranky flash pameti

			if (FLASH_ProgramWord(APPLICATION_SPACE_CRC, crc) == FLASH_COMPLETE)
			{
				// korektni zapis CRC
			}
			crc = FIRMWAREVALID;
			if (FLASH_ProgramWord(APPLICATION_SPACE_VALIDITY, crc) == FLASH_COMPLETE)
			{
				// korektni zapis FW valid priznaku
			}

			fu_state = FIRMUPDATE_FINAL;

			bootloader_send_message((uint8_t *)"FCRC_OK");
		}
	}
	else
	{
		bootloader_send_message((uint8_t *)"FCRC_NO");
		return RETURN_ERROR;
	}

	FLASH_Lock();

	return RETURN_NORMAL;
}

//
// restartuje bootloader
CMD_RETURN bootloader_restart(void *p_i)
{
	// restartovat je mozne z jakehokoliv stavu automatu

	// zalozeni ulohy restartu aplikace
	if(Scheduler_Add_Task(Reset_service, 0, RESET_SERVICE_PERIOD) == SCH_MAX_TASKS)
	{
		// chyba pri zalozeni service
		return RETURN_ERROR;
	}
	else
	{
		return RETURN_NORMAL;
	}
}

//
// inicializuje chunk binarni prenos
CMD_RETURN bootloader_chunk_request(void *p_i)
{
	// zapsani delky nasledujicich dat, prijem ve tvaru CHnn, ukazatel p_i ukazuje na znak 'C'
	uint16_t *p_int = p_i;
	chunk_len = p_int[1];	// nacti cislo ulozene od znaku s indexem 1 - treti byte v poradi

	if ((fu_state != FIRMUPDATE_PROCESS) || (chunk_len > RXBUFFERSIZE))
	{
		// pozadavek ve spatnem stavu - zaporna odpoved bez akce
		bootloader_send_message((uint8_t *)"CH_NO");
		return RETURN_ERROR;
	}
	else
	{
		// prepnuti do rezimu binarniho prenosu
		usart_set_mode(USARTBINARY);

		bootloader_send_message((uint8_t *)"CH_OK");

		return RETURN_NORMAL;
	}
}

uint8_t bootloader_password_check(uint8_t *text)
{
	uint8_t i;
	for (i = 0; i < PASSWORDLEN; i++)
	{
		if (text[i] != password[i])
		{
			return RETURN_ERROR;
		}
	}
	return RETURN_NORMAL;
}

void bootloader_memclear_fail(void)
{
	uint8_t text[] = "MCPR_NO";
	bootloader_sendreply(text);
}

uint8_t bootloader_memclear_do(void)
{
	uint8_t text[] = "MCPR_OK";
	uint32_t NbrOfPage = 0x00;
	uint32_t EraseCounter = 0x00;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	uint8_t retval = RETURN_NORMAL;

	// Unlock the Flash to enable the flash control register access
	FLASH_Unlock();

	// Clear pending flags (if any)
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

	// Define the number of page to be erased
	NbrOfPage = (APPLICATION_END_ADDRESS - APPLICATION_ADDRESS) / FLASH_PAGE_SIZE;

	// Erase the FLASH pages
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		if (FLASH_ErasePage(APPLICATION_ADDRESS + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE)
		{
			// Error occurred while sector erase.
			bootloader_memclear_fail();
			retval = RETURN_ERROR;
			return retval;
		}
	}

	// send reply
	bootloader_sendreply(text);

	return retval;
}

void bootloader_memlock_fail(void)
{
	uint8_t text[] = "MLREQ_NO";
	bootloader_sendreply(text);
}

void bootloader_mlreq(void)
{
	uint8_t text[] = "MLREQ_OK";

	FLASH_Lock();

	// send reply
	bootloader_sendreply(text);
}

void bootloader_crc_config(void)
{
	/* Enable CRC AHB clock interface */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

	/* DeInit CRC peripheral */
	CRC_DeInit();

	/* Init the INIT register */
	CRC_SetInitRegister(0);

	/* Select 32-bit polynomial size */
	CRC_PolynomialSizeSelect(CRC_PolSize_32);

	/* Set the polynomial coefficients */
	CRC_SetPolynomial(CRCPOLYNOME);
}

uint8_t bootloader_crc_check_block(uint32_t start, uint32_t size, uint32_t refcrc)
{
	uint8_t retval;

	// CRC32 cele pameti pro aplikaci = kod a zbytek prazdneho mista
	CRC_SetInitRegister(0);
	if (CRC_CalcBlockCRC((uint32_t *)start, size) == refcrc)
	{
		retval = RETURN_NORMAL;
	}
	else
	{
		retval = RETURN_ERROR;
	}

	return retval;
}

#ifdef CRCCMDPROTECTION
CMD_RETURN bootloader_crc_cmd_check(uint32_t *p_crc)
{
	if (bootloader_crc_check_block(p_crc, CRCLENGTH, p_crc[1]) == RETURN_ERROR)
	{
		// CRC nesouhlasi
		bootloader_crc_fail();
		return RETURN_ERROR;
	}
	return RETURN_NORMAL;
}
#endif // CRCCMDPROTECTION

void bootloader_send_message(uint8_t *text)
{
	bootloader_sendreply(text);
}

void bootloader_crc_fail(void)
{
	bootloader_send_message((uint8_t *)"CF");
}

uint16_t bootloader_get_chunk_len(void)
{
	return chunk_len;
}

CMD_RETURN bootloader_binary_process(void)
{
	// CRC kontrola dat
	uint8_t *p_byte = (uint8_t *)&Rx.buffer[chunk_len-CRCLENGTH];
	uint32_t crc = (p_byte[0] << 24) + (p_byte[1] << 16) + (p_byte[2] << 8) + p_byte[3];

	if (bootloader_crc_check_block((uint32_t)&Rx.buffer[0], (chunk_len/4)-1, crc) == RETURN_ERROR)
	{
		// CRC nesouhlasi
		bootloader_crc_fail();
		return RETURN_ERROR;
	}
	else
	{
		// CRC souhlasi
		return bootloader_flash_page();
	}
}

CMD_RETURN bootloader_flash_page(void)
{
	uint32_t page_addr = flash_address;
	uint32_t *p_word = (uint32_t *)&Rx.buffer[0];

	if ((page_addr % FLASH_PAGE_SIZE) != 0)
	{
		// spatna adresa zacatku stranky
		bootloader_send_message((uint8_t *)"CH_NO");
		return RETURN_ERROR;
	}

	while (page_addr < (flash_address+FLASH_PAGE_SIZE))
	{
		if (FLASH_ProgramWord(page_addr, *p_word) == FLASH_COMPLETE)
		{
			page_addr++;
		}
		else
		{
			// chyba zapisu do FLASH
			// vymazat stranku

			bootloader_send_message((uint8_t *)"CH_NO");
			return RETURN_ERROR;
		}
	}
	flash_address += FLASH_PAGE_SIZE;

	bootloader_send_message((uint8_t *)"CH_OK");

	return RETURN_NORMAL;
}
