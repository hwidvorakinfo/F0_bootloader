/*
 * usart.c
 *
 *  Created on: 3. 5. 2019
 *      Author: Petr Dvorak
 */

#include "usart.h"
#include "string.h"
#include "application.h"

volatile usart_data_tx_t Tx;
volatile usart_data_rx_t Rx;

static uint8_t usart_mode = USARTCOMMAND;

//#define RSHARE_SECTION ".rshared.data,\"aw\",%nobits//"
//#define _RSHARE __attribute__((used, section(RAMSHARE)))

static volatile uint32_t *shared_data = (uint32_t *)0x200000C0;

void usart_config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(USART_TX_CLOCKS | USART_RX_CLOCKS, ENABLE);

  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(USART_CLOCKS, ENABLE);

  /* Connect PA2 to USART2_Tx */
  GPIO_PinAFConfig(USART_TX_PORT, USART_TX_SOURCE, USART_TX_AF);

  /* Connect PA3 to USART2_Rx */
  GPIO_PinAFConfig(USART_RX_PORT, USART_RX_SOURCE, USART_RX_AF);

  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;
  GPIO_Init(USART_TX_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
  GPIO_Init(USART_RX_PORT, &GPIO_InitStructure);

#ifdef RS485_ENABLED
  // RS485 DE signal
  GPIO_InitStructure.GPIO_Pin = USART_DE_PIN;
  GPIO_Init(USART_DE_PORT, &GPIO_InitStructure);
#endif // RS485_ENABLED

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */

  USART_DeInit(USART);

#ifdef RS485_ENABLED
	#ifdef USE_OVERSAMPLING_8
	  #define OVERSAMPLING_VALUE 0x08
	  USART_OverSampling8Cmd(USART1, ENABLE);
	#else
	  #define OVERSAMPLING_VALUE 0x10
	#endif
#endif // RS485_ENABLED

  USART_InitStructure.USART_BaudRate = RS485BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART, &USART_InitStructure);

  /* NVIC configuration */
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = USART_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#ifdef RS485_ENABLED
  /* Enable driver enable mode */
  USART_DECmd(USART, ENABLE);

  /* Configure DE assertion time */
  USART_SetDEAssertionTime(USART, OVERSAMPLING_VALUE);

  /* Configure DE deassertion time */
  USART_SetDEDeassertionTime(USART, OVERSAMPLING_VALUE);

  /* Configure polarity of DE */
  USART_DEPolarityConfig(USART, USART_DEPolarity_High);
#endif

  /* Enable USART */
  USART_Cmd(USART, ENABLE);

  /* Enable the USARTx Receive interrupt: this interrupt is generated when the USARTx receive data register is not empty */
  USART_ITConfig(USART, USART_IT_RXNE, ENABLE);

  // inicializuj data prijmu a vysilani
  Tx.index = 0;
  Tx.status = READYTOSEND;
  Rx.index = 0;
  Rx.status = READYTORECEIVE;

  // vypis uvodni retezec
  // hello definovano v application.c
  extern const uint8_t hello[];

  usart_send_text((uint8_t *)&hello);
  usart_newline();

  // demo pro prenos sdilenych dat mezi aplikaci a bootloaderem
//  if (*shared_data == 7)
//  {
//	  volatile uint8_t shared[3];
//	  shared[0] = 'H';
//	  shared[1] = 'I';
//	  shared[2] = 0;
//	  usart_send_text((uint8_t *)&shared);
//  }
//  *shared_data = 3;
}

void usart_send_text(uint8_t *text)
{
	usart_wait_until_sent();

	Tx.index = 0;
	Tx.status = TRANSFERING;

	strcpy((char *)&Tx.buffer, (const char *)text);

	/* Enable the USARTx transmit data register empty interrupt */
	USART_ITConfig(USART, USART_IT_TXE, ENABLE);
}

void usart_newline(void)
{
	usart_wait_until_sent();

	uint8_t endline[] = "  ";

	Tx.index = 0;
	Tx.status = TRANSFERING;

	endline[0] = '\r';
	endline[1] = '\n';
	strcpy((char *)&Tx.buffer, (const char *)endline);

	/* Enable the USARTx transmit data register empty interrupt */
	USART_ITConfig(USART, USART_IT_TXE, ENABLE);
}

void usart_wait_until_sent(void)
{
	while (Tx.status == TRANSFERING)
	{;}
}


void usart_release_Rx_buffer(void)
{
	Rx.index = 0;
	Rx.status = READYTORECEIVE;
}

uint8_t *get_Rx_buffer(void)
{
	return (uint8_t *)&Rx.buffer;
}

void usart_set_mode(uint8_t mode)
{
	usart_mode = mode;
}

uint8_t usart_get_mode(void)
{
	return usart_mode;
}
