/*
 * services.c
 *
 *  Created on: Dec 28, 2014
 *      Author: Petr Dvorak
 */

#include "services.h"
#include "application.h"
#include "bootloader.h"

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


// obsluzna sluzba zavolana z IRQ rutiny UART1
void Command_service(void)
{
	// dekodovani a zpracovani zpravy z UART
	commands_process();

	// uvolneni Rx bufferu
	usart_release_Rx_buffer();
}

// startovaci sluzba aplikace
void AppStart_service(void)
{
	pFunction appEntry;
	uint32_t appStack;

	/* Get the application stack pointer (First entry in the application vector table) */
	appStack = (uint32_t) *((__IO uint32_t*)APPLICATION_ADDRESS);

	/* Get the application entry point (Second entry in the application vector table) */
	appEntry = (pFunction) *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);

	/* Set the application stack pointer */
	__set_MSP(appStack);

	/* Start the application */
	appEntry();

	while(1);
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
