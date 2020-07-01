/*
 * clock.c
 *
 *  Created on: Dec 28, 2014
 *      Author: daymoon
 */

/* Includes ------------------------------------------------------------------*/
#include "clock.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void Clock_config(void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
	     this is done through SystemInit() function which is called from startup
	     files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
	     startup_stm32f429_439xx.s/startup_stm32f401xx.s) before to branch to
	     application main. To reconfigure the default setting of SystemInit()
	     function, refer to system_stm32f4xx.c file
	*/

// jak nastavit hodiny:
// HSE_VALUE musi byt v peripheral library symbols jako define s hodnotou skutecne frekvence externich hodin
// HSE_VALUE musi byt v projektu jako symbol s hodnotou skutecne frekvence externich hodin
// https://stm32f4-discovery.net/2015/01/properly-set-clock-speed-stm32f4xx-devices/

	RCC_GetClocksFreq((RCC_ClocksTypeDef *)&RCC_Clocks);
}
