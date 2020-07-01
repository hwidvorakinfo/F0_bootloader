/*
 * gpio.c
 *
 *  Created on: 14. 8. 2019
 *      Author: Petr Dvorak
 */

#include "gpio.h"
#include "application.h"

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(OUTPUT_CLOCKS, ENABLE);

	// GPIO vystupy na PAx
#ifdef GPIOA_INIT
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_A_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		// 50 MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_A, &GPIO_InitStructure);
	// inicializuj vystupy na PAx na spravne hodnoty
	#ifdef GPIOA_RESET_INIT
	GPIO_ResetBits(OUT_PORT_A, OUT_PORT_A_RESET_INIT);
	#endif
	#ifdef GPIOA_SET_INIT
	GPIO_SetBits(OUT_PORT_A, OUT_PORT_A_SET_INIT);
	#endif
#endif

	// GPIO vystupy na PBx
#ifdef GPIOB_INIT
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_B_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_B, &GPIO_InitStructure);
	// inicializuj vystupy na PBx na spravne hodnoty
	#ifdef GPIOB_RESET_INIT
	GPIO_ResetBits(OUT_PORT_B, OUT_PORT_B_RESET_INIT);
	#endif
	#ifdef GPIOB_SET_INIT
	GPIO_SetBits(OUT_PORT_B, OUT_PORT_B_SET_INIT);
	#endif
#endif

#ifdef GPIOC_INIT
	// GPIO vystupy na PCx
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_C_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		// 50 MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_C, &GPIO_InitStructure);
	// inicializuj vystupy na PCx na spravne hodnoty
	#ifdef GPIOC_RESET_INIT
	GPIO_ResetBits(OUT_PORT_C, OUT_PORT_C_RESET_INIT);
	#endif
	#ifdef GPIOC_SET_INIT
	GPIO_SetBits(OUT_PORT_C, OUT_PORT_C_SET_INIT);
	#endif
#endif

#ifdef GPIOD_INIT
	// GPIO vystupy na PDx
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_D_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		// 50 MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_D, &GPIO_InitStructure);
	// inicializuj vystupy na PDx na spravne hodnoty
	#ifdef GPIOD_RESET_INIT
	GPIO_ResetBits(OUT_PORT_D, OUT_PORT_D_RESET_INIT);
	#endif
	#ifdef GPIOD_SET_INIT
	GPIO_SetBits(OUT_PORT_D, OUT_PORT_D_SET_INIT);
	#endif
#endif

#ifdef GPIOE_INIT
	// GPIO vystupy na PEx
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_E_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		// 50 MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_E, &GPIO_InitStructure);
	// inicializuj vystupy na PDx na spravne hodnoty
	#ifdef GPIOE_RESET_INIT
	GPIO_ResetBits(OUT_PORT_E, OUT_PORT_E_RESET_INIT);
	#endif
	#ifdef GPIOE_SET_INIT
	GPIO_SetBits(OUT_PORT_E, OUT_PORT_E_SET_INIT);
	#endif
#endif

#ifdef GPIOF_INIT
	// GPIO vystupy na PFx
	GPIO_InitStructure.GPIO_Pin = OUT_PORT_F_OUTPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		// 50 MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(OUT_PORT_F, &GPIO_InitStructure);
	// inicializuj vystupy na PFx na spravne hodnoty
	#ifdef GPIOF_RESET_INIT
	GPIO_ResetBits(OUT_PORT_F, OUT_PORT_F_RESET_INIT);
	#endif
	#ifdef GPIOF_SET_INIT
	GPIO_SetBits(OUT_PORT_F, OUT_PORT_F_SET_INIT);
	#endif
#endif

#ifdef GPIOA_INPUT
	// GPIO vstupy na PAx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_A_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_A, &GPIO_InitStructure);
#endif

#ifdef GPIOB_INPUT
	// GPIO vstupy na PBx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_B_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_B, &GPIO_InitStructure);
#endif

#ifdef GPIOC_INPUT
	// GPIO vstupy na PCx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_C_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_C, &GPIO_InitStructure);
#endif

#ifdef GPIOD_INPUT
	// GPIO vstupy na PDx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_D_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_D, &GPIO_InitStructure);
#endif

#ifdef GPIOE_INPUT
	// GPIO vstupy na PEx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_E_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_E, &GPIO_InitStructure);
#endif

#ifdef GPIOF_INPUT
	// GPIO vstupy na PFx
	GPIO_InitStructure.GPIO_Pin = IN_PORT_F_INPUTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(IN_PORT_F, &GPIO_InitStructure);
#endif
}

