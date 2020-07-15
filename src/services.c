/*
 * services.c
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#include "services.h"
#include "application.h"
#include "bootloader.h"
#include "errorcodes.h"

/* private prototypes */

void LED_service(void)
{
	uint16_t led_port;

//	led_port = GPIO_ReadOutputData(STATUS_LED_PORT);
//	if (led_port & STATUS_LED)
//	{
//		GPIO_ResetBits(STATUS_LED_PORT, STATUS_LED);		// stavova LED off
//	}
//	else
//	{
//		GPIO_SetBits(STATUS_LED_PORT, STATUS_LED);			// stavova LED on
//	}
}

void Delay_service(void)
{
	Set_Delay_finished(DELAY_FINISHED);
}


// obsluzna sluzba zavolana z IRQ rutiny UART
void Command_service(void)
{
	// dekodovani a zpracovani zpravy z UART
	commands_process();

	// uvolneni Rx bufferu
	usart_release_Rx_buffer();
}

// obsluzna sluzba zavolana z IRQ rutiny UART
void Binary_data_service(void)
{
	// dekodovani a zpracovani zpravy z UART
	if (bootloader_binary_process() != RETURN_NORMAL)
	{
		// chyba prubehu zpracovani binarni zpravy
	}

	// uvolneni Rx bufferu
	usart_release_Rx_buffer();

	// nastaveni textoveho rezim pro prikazy
	usart_set_mode(USARTCOMMAND);
}

// startovaci sluzba aplikace
void AppStart_service(void)
{
	pFunction appEntry;
	uint32_t appStack;
	uint32_t *appcrc = (uint32_t *)APPLICATION_SPACE_CRC;

	// pokud sedi CRC cele aplikace, spust ji
	if (bootloader_crc_check_block(APPLICATION_ADDRESS, APPLICATION_SPACE/sizeof(uint32_t), *appcrc) == RETURN_NORMAL)
	{
		/* Get the application stack pointer (First entry in the application vector table) */
		appStack = (uint32_t) *((__IO uint32_t*)APPLICATION_ADDRESS);

		/* Get the application entry point (Second entry in the application vector table) */
		appEntry = (pFunction) *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);

		/* Set the application stack pointer */
		__set_MSP(appStack);

		/* Start the application */
		appEntry();

		// program by nemel nikdy dospet sem
		while(1)
		{
			// assert error
			assert_param(0);
		}
	}
}

// resetujici sluzba
void Reset_service(void)
{
	NVIC_SystemReset();
}

// obsluzna sluzba heartbeat zprav
void Heartbeat_service(void)
{
//#define HBALENABLED
#ifdef HBALENABLED

	uint8_t text[] = "HBAL";

	// posli zpravu
	usart_send_text((uint8_t *)&text);				// odesli zpravu HBAL
	usart_newline();
#endif
}
