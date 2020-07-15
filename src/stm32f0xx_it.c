/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern usart_data_tx_t Tx;
extern usart_data_rx_t Rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	while(1)
	{
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	while(1)
	{
	}
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	while(1)
	{
	}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	// spust pruchod schedulerem
	Run_scheduler();
}

/**
* @brief  This function handles USART interrupt request.
* @param  None
* @retval None
*/
void USART2_IRQHandler(void)
{
  /* USART in mode Transmitter -------------------------------------------------*/
  if (USART_GetITStatus(USART, USART_IT_TXE) == SET)
  {
        USART_SendData(USART, Tx.buffer[Tx.index++]);
        if (Tx.buffer[Tx.index] == 0)
        {
        	// konec prenosu
        	Tx.status = TRANSFERDONE;

        	/* Disable the USARTx transmit data register empty interrupt */
        	USART_ITConfig(USART, USART_IT_TXE, DISABLE);
        }
  }

  /* USART in mode Receiver --------------------------------------------------*/
  if (USART_GetITStatus(USART, USART_IT_RXNE) == SET)
  {
	  if (Rx.status != RECEIVEDONE)
	  {
		  // prijmi byte dat
		  Rx.buffer[Rx.index] = USART_ReceiveData(USART);

		  // v jakem jsi rezimu? rozhodni o ukonceni prijmu
		  if (usart_get_mode() == USARTCOMMAND)
		  {
			  // textovy rezim
			  if ((Rx.buffer[Rx.index] == '\r') || (Rx.buffer[Rx.index] == '\n'))
			  {
				  // konec prenosu
				  Rx.status = RECEIVEDONE;
				  Rx.buffer[Rx.index] = 0;

				  // zalozeni sluzby pro dekodovani a zpracovani zpravy
				  if(Scheduler_Add_Task(Command_service, 0, 0) == SCH_MAX_TASKS)
				  {
					// chyba pri zalozeni service
				  }
				  return;
			  }
		  }
		  else
		  {
			  // binarni blokovy rezim
			  if (Rx.index == (bootloader_get_chunk_len()-1))
			  {
				  // konec prenosu
				  Rx.status = RECEIVEDONE;

				  // zalozeni sluzby pro dekodovani a zpracovani zpravy
				  if(Scheduler_Add_Task(Binary_data_service, 0, 0) == SCH_MAX_TASKS)
				  {
					// chyba pri zalozeni service
				  }
				  return;
			  }
		  }

		  // zpracuj prijem dalsiho znaku
		  Rx.index++;
		  // neni prekrocena maximalni delka?
		  if (Rx.index > RXBUFFERSIZE)
		  {
			  // je prekrocena, ukonci prijem a cti dale od zacatku
			  Rx.status = READYTORECEIVE;
			  Rx.index = 0;
			  Rx.buffer[RXBUFFERSIZE-1] = 0;
		  }

//		  if ((Rx.buffer[Rx.index] == '\r') || (Rx.buffer[Rx.index] == '\n'))
//		  {
//			  // konec prenosu
//			  Rx.status = RECEIVEDONE;
//			  Rx.buffer[Rx.index] = 0;
//
//			  // zalozeni sluzby pro dekodovani a zpracovani zpravy
//			  if(Scheduler_Add_Task(Command_service, 0, 0) == SCH_MAX_TASKS)
//			  {
//				// chyba pri zalozeni service
//			  }
//		  }
//		  else
//		  {
//			  // prijmi dalsi znak
//			  Rx.index++;
//			  // neni prekrocena maximalni delka?
//			  if (Rx.index > RXBUFFERSIZE)
//			  {
//				  // je prekrocena, ukonci prijem a cti dale od zacatku
//				  Rx.status = READYTORECEIVE;
//				  Rx.index = 0;
//				  Rx.buffer[RXBUFFERSIZE-1] = 0;
//			  }
//		  }

	  }
  }
}

/**
  * @brief  This function handles EXTI0_1_IRQHandler global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
	// preruseni od kanalu na kterem je PANIC signal
	if(EXTI_GetITStatus(BUTTON0_EXTI_LINE) != RESET)
	{


		/* Clear the BUTTON0_EXTI_LINE line pending bit */
		EXTI_ClearITPendingBit(BUTTON0_EXTI_LINE);
	}
}

/**
  * @brief  This function handles EXTI2_3_IRQHandler global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_3_IRQHandler(void)
{
	// preruseni od kanalu na kterem je PANIC signal
	if(EXTI_GetITStatus(BUTTON1_EXTI_LINE) != RESET)
	{


		/* Clear the BUTTON1_EXTI_LINE line pending bit */
		EXTI_ClearITPendingBit(BUTTON1_EXTI_LINE);
	}
}

/**
  * @brief  This function handles EXTI4_15_IRQHandler global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
	// preruseni od kanalu na kterem je PANIC signal
	if(EXTI_GetITStatus(PANIC_EXTI_LINE) != RESET)
	{
		// zakaz okamzite topeni


		/* Clear the EXTI PANIC line pending bit */
		EXTI_ClearITPendingBit(PANIC_EXTI_LINE);
	}
	else
	if(EXTI_GetITStatus(PICKED0_EXTI_LINE) != RESET)
	{

		/* Clear the PICKED0_EXTI_LINE line pending bit */
		EXTI_ClearITPendingBit(PICKED0_EXTI_LINE);
	}
	else
	if(EXTI_GetITStatus(PICKED1_EXTI_LINE) != RESET)
	{

		/* Clear the PICKED1_EXTI_LINE line pending bit */
		EXTI_ClearITPendingBit(PICKED1_EXTI_LINE);
	}

}



/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
