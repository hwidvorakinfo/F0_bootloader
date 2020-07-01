/*
 * spi.h
 *
 *  Created on: 14. 8. 2019
 *      Author: Petr Dvorak
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx.h"
#include "defs.h"
#include "stm32f0xx_conf.h"

#define TRANSFER_DONE			0
#define TRANSFER_STARTED		10

typedef enum {
	SPI_UNUSED,
	SPI_TOUCHPANEL,
	SPI_WIFI,
	SPI_CODEC
} SPI_SEMAPHORE;

void spi_init(void);
uint8_t SPI_ReadWrite(uint8_t writedat);
void SPI_Write(uint8_t writedat);
void SPI_Write16(uint16_t writedat);


#endif /* SPI_H_ */
