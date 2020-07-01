/*
 * dwarf.h
 *
 *  Created on: 26. 6. 2020
 *      Author: Petr Dvorak
 */

#ifndef DWARF_H_
#define DWARF_H_

#include "stm32f0xx.h"
#include "defs.h"
#include "stm32f0xx_conf.h"
#include "application.h"

typedef enum {
	THERM0 = 0,
	THERM1,
	PT100,
	HEATER,
} ADC_CHANNELS;

#define RGBLED_RED					1
#define RGBLED_GREEN				2
#define RGBLED_BLUE					3

#define RGBLED_PWMFREQ				150000
#define RGBLED_PWM_DUTY_INIT		0
#define RGBLED_PWM_DUTY_MAX			9500
#define RGBLED_PWM_DUTY_MIN			0
#define RGBLED_PWM_MAX				10000
#define RGBLED_PWM_DUTY_STEP		100

#define WHITELED_PWMFREQ			150000
#define WHITELED_PWM_DUTY_INIT		0
#define WHITELED_PWM_DUTY_MAX		9500
#define WHITELED_PWM_DUTY_MIN		0
#define WHITELED_PWM_MAX			10000
#define WHITELED_PWM_DUTY_STEP		100

#define FAN_PWMFREQ					1500
#define FAN_PWM_DUTY_INIT			0
#define FAN_PWM_DUTY_MAX			9500
#define FAN_PWM_DUTY_MIN			0
#define FAN_PWM_MAX					10000
#define FAN_PWM_DUTY_STEP			100

#define HEATER_PWMFREQ				4700
#define HEATER_PWM_DUTY_INIT		0
#define HEATER_PWM_DUTY_MAX			9500
#define HEATER_PWM_DUTY_MIN			0
#define HEATER_PWM_MAX				10000
#define HEATER_PWM_DUTY_STEP		100

#define RGBLED_TIMER				TIM1
#define RGBLED_TIMER_CLOCKS			RCC_APB2Periph_TIM1
#define RGBLED_TIM_PRESCALER		0
#define RGBLED_TIMER_IRQ			TIM1_BRK_UP_TRG_COM_IRQn
#define RGBLED_TIMER_CC				TIM_IT_CC1
#define RGBLED_TIMER_IRQ_PRIORITY	1
#define RGBLED_TIMER_PULSE			3

#define WHITELED_TIMER				TIM1
#define WHITELED_TIMER_CLOCKS		RCC_APB2Periph_TIM1
#define WHITELED_TIM_PRESCALER		0
#define WHITELED_TIMER_IRQ			TIM1_BRK_UP_TRG_COM_IRQn
#define WHITELED_TIMER_CC			TIM_IT_CC1
#define WHITELED_TIMER_IRQ_PRIORITY	1
#define WHITELED_TIMER_PULSE		3

#define FAN_TIMER					TIM3
#define FAN_TIMER_CLOCKS			RCC_APB1Periph_TIM3
#define FAN_TIM_PRESCALER			0
#define FAN_TIMER_IRQ				TIM3_IRQn
#define FAN_TIMER_CC				TIM_IT_CC1
#define FAN_TIMER_IRQ_PRIORITY		1
#define FAN_TIMER_PULSE				3

#define HEATER_TIMER				TIM16
#define HEATER_TIMER_CLOCKS			RCC_APB2Periph_TIM16
#define HEATER_TIM_PRESCALER		0
#define HEATER_TIMER_IRQ			TIM16_IRQn
#define HEATER_TIMER_CC				TIM_IT_CC1
#define HEATER_TIMER_IRQ_PRIORITY	1
#define HEATER_TIMER_PULSE			3

#define RGBLED_PORT_CLOCKS			RCC_AHBPeriph_GPIOA
#define RGBLED_PORT					GPIOA
#define RGBLED_R					GPIO_Pin_8
#define RGBLED_G					GPIO_Pin_9
#define RGBLED_B					GPIO_Pin_10
#define RGBLED_R_PIN_SOURCE			GPIO_PinSource8
#define RGBLED_G_PIN_SOURCE			GPIO_PinSource9
#define RGBLED_B_PIN_SOURCE			GPIO_PinSource10

#define WHITELED_PORT_CLOCKS		RCC_AHBPeriph_GPIOA
#define WHITELED_PORT				GPIOA
#define WHITELED					GPIO_Pin_11
#define WHITELED_PIN_SOURCE			GPIO_PinSource11

#define FAN0						1
#define FAN1						2
#define FAN_PORT_CLOCKS				RCC_AHBPeriph_GPIOC
#define FAN_PORT					GPIOC
#define FAN0_PIN					GPIO_Pin_6
#define FAN0_PIN_SOURCE				GPIO_PinSource6
#define FAN1_PIN					GPIO_Pin_7
#define FAN1_PIN_SOURCE				GPIO_PinSource7

#define HEATER_PORT_CLOCKS			RCC_AHBPeriph_GPIOA
#define HEATER_PORT					GPIOA
#define HEATER_PIN					GPIO_Pin_6
#define HEATER_PIN_SOURCE			GPIO_PinSource6


extern uint16_t *p_adc_data;

void dwarf_init(void);
void dwarf_rgbled_init(void);
void dwarf_rgbled_set_duty(uint8_t color, uint16_t duty);
void dwarf_whiteled_init(void);
void dwarf_whiteled_set_duty(uint16_t duty);
void dwarf_fanx_init(uint8_t fan);
void dwarf_fanx_set_duty(uint8_t fan, uint16_t duty);
uint16_t dwarf_fanx_get_value(uint8_t fan);
void dwarf_heater_init(void);
void dwarf_heater_set_duty(uint16_t duty);

#endif /* DWARF_H_ */
