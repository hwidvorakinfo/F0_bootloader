/*
 * bootloader.h
 *
 *  Created on: 1. 7. 2020
 *      Author: Petr Dvorak
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "stm32f0xx.h"
#include "defs.h"

/* Application start address */
#define APPDATA_ADDRESS				((uint32_t)0x08003000)
#define APPLICATION_ADDRESS			((uint32_t)0x08003400)
#define APPLICATION_SPACE			((uint32_t)(0x10000-0x3400))
#define APPLICATION_END_ADDRESS     ((uint32_t)0x08010000)   /* End @ of user Flash area */
#define FLASH_PAGE_SIZE				((uint32_t)0x00000400)   /* FLASH Page Size */
#define APPLICATION_SPACE_CRC		((uint32_t)0x08003000)
#define APPLICATION_SPACE_VALIDITY	((uint32_t)0x08003004)

#define FIRMWAREVALID				0x10203040

typedef void (*pFunction)(void);

// bootloader version
#define BLVERSION					"0.01"

// CRC command protection
//#define CRCCMDPROTECTION
#define CRCLENGTH					(4)


typedef enum {
	FIRMUPDATE_IDLE = 0,
	FIRMUPDATE_REQUESTED,
	FIRMUPDATE_CLEAR_REQ,
	FIRMUPDATE_CLEAR_FAILED,
	FIRMUPDATE_PROCESS,
	FIRMUPDATE_FINAL,
	FIRMUPDATE_LOCKED,
	FIRMUPDATE_ERROR
} AUTOMAT_STATE;

typedef uint8_t CMD_RETURN;

#define UNITEXTLEN					(20)
#define PASSWORDLEN					(6)
#define CRCPOLYNOME					(0x4C11DB7)

uint8_t bootloader_init(void);
void bootloader_set_state(AUTOMAT_STATE newstate);
AUTOMAT_STATE bootloader_get_state(void);
uint16_t bootloader_get_chunk_len(void);
CMD_RETURN bootloader_binary_process(void);
CMD_RETURN bootloader_flash_page(void);
#ifdef CRCCMDPROTECTION
CMD_RETURN bootloader_crc_cmd_check(void);
#endif // CRCCMDPROTECTION
uint8_t bootloader_crc_check_block(uint32_t start, uint32_t size, uint32_t refcrc);


CMD_RETURN bootloader_getversion(void *p_i);
CMD_RETURN bootloader_mem_clear(void *p_i);
CMD_RETURN bootloader_fureq(void *p_i);
CMD_RETURN bootloader_mem_lock(void *p_i);
CMD_RETURN bootloader_update_stop(void *p_i);
CMD_RETURN bootloader_final_check(void *p_i);
CMD_RETURN bootloader_restart(void *p_i);
CMD_RETURN bootloader_chunk_request(void *p_i);


uint8_t bootloader_password_check(uint8_t *text);
void bootloader_memclear_fail(void);
uint8_t bootloader_memclear_do(void);
void bootloader_mlreq(void);
void bootloader_memlock_fail(void);
void bootloader_send_message(uint8_t *text);




#endif /* BOOTLOADER_H_ */
