/*
 * bootloader.h
 *
 *  Created on: 1. 7. 2020
 *      Author: Petr Dvorak
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

/* Application start address */
#define APPLICATION_ADDRESS        0x08003000

typedef void (*pFunction)(void);



void bootloader_init(void);

#endif /* BOOTLOADER_H_ */
