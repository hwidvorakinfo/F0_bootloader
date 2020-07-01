/*
 * spi.c
 *
 *  Created on: 14. 8. 2019
 *      Author: Petr Dvorak
 */

#include "spi.h"
#include "application.h"

#ifdef SPI

static SPI_SEMAPHORE spi_semaphore;
static uint8_t dma_spi_status = TRANSFER_DONE;

void spi_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	// hodiny pro piny a periferii
	RCC_AHB1PeriphClockCmd(LCD_MOSI_CLK | LCD_SCK_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(LCD_SPI_CLK, ENABLE);

	// piny SPI
	GPIO_InitStructure.GPIO_Pin = LCD_SCK_PIN | LCD_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_SCK_PORT, &GPIO_InitStructure);

	// Alternativni funkce
	GPIO_PinAFConfig(LCD_SCK_PORT, LCD_SCK_SOURCE, LCD_SCK_AF);
	GPIO_PinAFConfig(LCD_MOSI_PORT, LCD_MOSI_SOURCE, LCD_MOSI_AF);
	//GPIO_PinAFConfig(LCD_MISO_PORT, LCD_MISO_SOURCE, LCD_MISO_AF);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(LCD_SPI);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	// APB1=42MHz, optimalni je frekvence kolem 4MHz pro dlouhe vedeni
	// 42000kHz/8=5.25MHz
	// 42000kHz/16=2.63MHz
	// 42000kHz/4=10.5MHz
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(LCD_SPI, &SPI_InitStructure);
	SPI_Cmd(LCD_SPI, ENABLE); /* LCD_SPI enable */

	/* Drain SPI2 */
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{}
	SPI_I2S_ReceiveData(LCD_SPI);
}

uint8_t SPI_ReadWrite(uint8_t writedat)
{
	uint16_t i = 0;

    /* Loop while DR register in not empty */
    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
    SPI_I2S_SendData(LCD_SPI, writedat);

    /* Wait to receive a byte */
    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    i = SPI_I2S_ReceiveData(LCD_SPI) ;

    /* Return the byte read from the SPI bus */
    return i;
}

void SPI_Write(uint8_t writedat)
{
    /* Loop while DR register is not empty */
    //while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET);
    //while ((LCD_SPI->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);

    /* Send byte through the SPI2 peripheral */
    //SPI_I2S_SendData(LCD_SPI, writedat);
    LCD_SPI->DR = writedat;

    /* Loop while DR register is not empty */
    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET);
}

void SPI_Write16(uint16_t writedat)
{
	SPI_Write((writedat & 0xFF00) >> 8);
	SPI_Write(writedat & 0x00FF);
}

void set_spi_semaphore(SPI_SEMAPHORE state)
{
	spi_semaphore = state;
}

SPI_SEMAPHORE get_spi_semaphore(void)
{
	return spi_semaphore;
}

void Set_DMA_SPI_transfer_status(uint8_t status)
{
	dma_spi_status = status;
}

uint8_t Get_DMA_SPI_transfer_status(void)
{
	return dma_spi_status;
}

#endif


