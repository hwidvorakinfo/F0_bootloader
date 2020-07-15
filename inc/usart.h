/*
 * usart.h
 *
 *  Created on: 3. 5. 2019
 *      Author: Petr Dvorak
 */

#ifndef USART_H_
#define USART_H_

#include <regulator_types.h>
#include "stm32f0xx.h"
#include "defs.h"

#define RXBUFFERSIZE				2048
#define TXBUFFERSIZE				128

#define USARTBINARY					(0x0F)
#define USARTCOMMAND				(0x00)

#define USART_TX_CLOCKS				RCC_AHBPeriph_GPIOA
#define USART_TX_PORT				GPIOA
#define USART_TX_PIN				GPIO_Pin_2
#define USART_TX_SOURCE				GPIO_PinSource2
#define USART_TX_AF					GPIO_AF_1

#define USART_RX_CLOCKS				RCC_AHBPeriph_GPIOA
#define USART_RX_PORT				GPIOA
#define USART_RX_PIN				GPIO_Pin_3
#define USART_RX_SOURCE				GPIO_PinSource3
#define USART_RX_AF					GPIO_AF_1

#define USART_DE_CLOCKS				RCC_AHBPeriph_GPIOA
#define USART_DE_PORT				GPIOA
#define USART_DE_PIN				GPIO_Pin_1
#define USART_DE_SOURCE				GPIO_PinSource1
#define USART_DE_AF					GPIO_AF_1

#define RS485_ENABLED
#ifdef RS485_ENABLED
 #define USE_OVERSAMPLING_8
#endif // RS485_ENABLED

#define USART_CLOCKS				RCC_APB1Periph_USART2
#define USART						USART2
#define USART_IRQn					USART2_IRQn
#define USART_IRQHandler			USART2_IRQHandler
#define USART_PRIORITY				0x03

typedef enum {
	READYTOSEND = 0,
	READYTORECEIVE,
	TRANSFERING,
	RECEIVING,
	TRANSFERDONE,
	RECEIVEDONE
} USARTSTATUS;

typedef struct {
	uint32_t dummy;					// to allign buffer to allow word addressing
	uint8_t buffer[TXBUFFERSIZE];
	uint16_t index;
	USARTSTATUS status;
} usart_data_tx_t;

typedef struct {
	uint32_t dummy;					// to allign buffer to allow word addressing
	uint8_t buffer[RXBUFFERSIZE];
	uint16_t index;
	USARTSTATUS status;
} usart_data_rx_t;


void usart_config(void);
void usart_send_text(uint8_t *text);
void usart_newline(void);
void usart_wait_until_sent(void);
void usart_release_Rx_buffer();
uint8_t *get_Rx_buffer(void);
void usart_set_mode(uint8_t mode);
uint8_t usart_get_mode(void);

#endif /* USART_H_ */
