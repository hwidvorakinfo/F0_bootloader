
#include "dwarf.h"
#include "application.h"
#include "services.h"

static uint16_t adc_dma_data[IN_ADC_COUNT];
uint16_t *p_adc_data = (uint16_t *)&adc_dma_data;
static uint16_t fan0_value;
static uint16_t fan1_value;
static uint16_t heater_value;

// HWID
static volatile uint8_t hwid_version = 0;

void dwarf_init(void)
{
	// HWID config
	// mozne verze v promenne hbl_hw_version
	// HWVERSION_V01  = 000 - HW verze v0.1/06-2020 ze dne 25-06-2020
	hwid_version = GPIO_ReadInputData(HWID_PORT) & HWID_MASK;

	// RGB LED init
	dwarf_rgbled_init();

	// WHITE picked sensor LED
	dwarf_whiteled_init();

	// FAN0 init
	dwarf_fanx_init(FAN0);

	// FAN1 init
	dwarf_fanx_init(FAN1);

	// Heater init
	dwarf_heater_init();


}

void dwarf_rgbled_init(void)
{
	volatile uint32_t TimerPeriod = 0;
	volatile uint16_t Channel1Pulse = 0;
	volatile uint16_t Channel2Pulse = 0;
	volatile uint16_t Channel3Pulse = 0;
	uint32_t pwmfreq = RGBLED_PWMFREQ;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RGBLED_PORT_CLOCKS, ENABLE);

	// AF PORTY VYSTUPU CASOVACE
	// R
	GPIO_InitStructure.GPIO_Pin = RGBLED_R;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RGBLED_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(RGBLED_PORT, RGBLED_R);				// R off

	// G
	GPIO_InitStructure.GPIO_Pin = RGBLED_G;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RGBLED_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(RGBLED_PORT, RGBLED_G);				// G off

	// B
	GPIO_InitStructure.GPIO_Pin = RGBLED_B;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RGBLED_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(RGBLED_PORT, RGBLED_B);				// B off

	GPIO_PinAFConfig(RGBLED_PORT, RGBLED_R_PIN_SOURCE, GPIO_AF_2);	// R jako TIM1_CH1
	GPIO_PinAFConfig(RGBLED_PORT, RGBLED_G_PIN_SOURCE, GPIO_AF_2);	// R jako TIM1_CH2
	GPIO_PinAFConfig(RGBLED_PORT, RGBLED_B_PIN_SOURCE, GPIO_AF_2);	// R jako TIM1_CH3


	  /* TIM1 Configuration ---------------------------------------------------
	   Generate 7 PWM signals with 4 different duty cycles:
	   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)
	    => TIM1CLK = PCLK2 = SystemCoreClock
	   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	   SystemCoreClock is set to 48 MHz for STM32F0xx devices

	   The objective is to generate 7 PWM signal at 17.57 KHz:
	     - TIM1_Period = (SystemCoreClock / 17570) - 1
	   The channel 1 and channel 1N duty cycle is set to 50%
	   The channel 2 and channel 2N duty cycle is set to 37.5%
	   The channel 3 and channel 3N duty cycle is set to 25%
	   The channel 4 duty cycle is set to 12.5%
	   The Timer pulse is calculated as follows:
	     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

	   Note:
	    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	    function to update SystemCoreClock variable value. Otherwise, any configuration
	    based on this variable will be incorrect.
	  ----------------------------------------------------------------------- */

	/* Compute the value to be set in ARR regiter to generate signal frequency at pwmfreq Hz */
	TimerPeriod = (SYSTEMCORECLOCK / pwmfreq ) - 1;

	/* Compute CCR1 value to generate a duty cycle at PWM_DUTY_INIT%  for channel 3 and 3N */
	Channel1Pulse = (uint16_t) (((uint32_t) RGBLED_PWM_DUTY_INIT * (TimerPeriod - 1)) / RGBLED_PWM_MAX);

	/* Compute CCR2 value to generate a duty cycle at PWM_DUTY_INIT%  for channel 3 and 3N */
	Channel2Pulse = (uint16_t) (((uint32_t) RGBLED_PWM_DUTY_INIT * (TimerPeriod - 1)) / RGBLED_PWM_MAX);

	/* Compute CCR3 value to generate a duty cycle at PWM_DUTY_INIT%  for channel 3 and 3N */
	Channel3Pulse = (uint16_t) (((uint32_t) RGBLED_PWM_DUTY_INIT * (TimerPeriod - 1)) / RGBLED_PWM_MAX);

	/* PWM_TIM clock enable */
	RCC_APB2PeriphClockCmd(RGBLED_TIMER_CLOCKS, ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = RGBLED_TIM_PRESCALER;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(RGBLED_TIMER, (TIM_TimeBaseInitTypeDef *)&TIM_TimeBaseStructure);

	/* Channel 3 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(RGBLED_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(RGBLED_TIMER, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(RGBLED_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(RGBLED_TIMER, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(RGBLED_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(RGBLED_TIMER, TIM_OCPreload_Enable);

	/* PWM_TIM counter enable */
	TIM_Cmd(RGBLED_TIMER, ENABLE);

	/* PWM_TIM Main Output Enable */
	TIM_CtrlPWMOutputs(RGBLED_TIMER, ENABLE);
}

void dwarf_whiteled_init(void)
{
	volatile uint32_t TimerPeriod = 0;
	volatile uint16_t Channel4Pulse = 0;
	uint32_t pwmfreq = WHITELED_PWMFREQ;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(WHITELED_PORT_CLOCKS, ENABLE);

	// AF PORTY VYSTUPU CASOVACE
	// White
	GPIO_InitStructure.GPIO_Pin = WHITELED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(WHITELED_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(WHITELED_PORT, WHITELED);				// R off

	GPIO_PinAFConfig(WHITELED_PORT, WHITELED_PIN_SOURCE, GPIO_AF_2);	// W jako TIM1_CH4


	  /* TIM1 Configuration ---------------------------------------------------
	   Generate 7 PWM signals with 4 different duty cycles:
	   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)
	    => TIM1CLK = PCLK2 = SystemCoreClock
	   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	   SystemCoreClock is set to 48 MHz for STM32F0xx devices

	   The objective is to generate 7 PWM signal at 17.57 KHz:
	     - TIM1_Period = (SystemCoreClock / 17570) - 1
	   The channel 1 and channel 1N duty cycle is set to 50%
	   The channel 2 and channel 2N duty cycle is set to 37.5%
	   The channel 3 and channel 3N duty cycle is set to 25%
	   The channel 4 duty cycle is set to 12.5%
	   The Timer pulse is calculated as follows:
	     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

	   Note:
	    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	    function to update SystemCoreClock variable value. Otherwise, any configuration
	    based on this variable will be incorrect.
	  ----------------------------------------------------------------------- */

	/* Compute the value to be set in ARR regiter to generate signal frequency at pwmfreq Hz */
	TimerPeriod = (SYSTEMCORECLOCK / pwmfreq ) - 1;

	/* Compute CCR4 value to generate a duty cycle at PWM_DUTY_INIT%  for channel 4 and 3N */
	Channel4Pulse = (uint16_t) (((uint32_t) WHITELED_PWM_DUTY_INIT * (TimerPeriod - 1)) / WHITELED_PWM_MAX);

	/* PWM_TIM clock enable */
	RCC_APB2PeriphClockCmd(WHITELED_TIMER_CLOCKS, ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = WHITELED_TIM_PRESCALER;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(WHITELED_TIMER, (TIM_TimeBaseInitTypeDef *)&TIM_TimeBaseStructure);

	/* Channel 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
	TIM_OC4Init(WHITELED_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(WHITELED_TIMER, TIM_OCPreload_Enable);

	/* PWM_TIM counter enable */
	TIM_Cmd(WHITELED_TIMER, ENABLE);

	/* PWM_TIM Main Output Enable */
	TIM_CtrlPWMOutputs(WHITELED_TIMER, ENABLE);
}


void dwarf_rgbled_set_duty(uint8_t color, uint16_t duty)
{
	/*
	dwarf_rgbled_set_duty(RGBLED_RED, 0);
	dwarf_rgbled_set_duty(RGBLED_GREEN, 0);
	dwarf_rgbled_set_duty(RGBLED_BLUE, 0);

	dwarf_rgbled_set_duty(RGBLED_RED, 5000);	// 50%
	dwarf_rgbled_set_duty(RGBLED_RED, 0);

	dwarf_rgbled_set_duty(RGBLED_GREEN, 5000);
	dwarf_rgbled_set_duty(RGBLED_GREEN, 0);

	dwarf_rgbled_set_duty(RGBLED_BLUE, 5000);
	dwarf_rgbled_set_duty(RGBLED_BLUE, 0);
	 */

	volatile uint16_t ChannelPulse = 0;
	volatile uint16_t TimerPeriod = (SYSTEMCORECLOCK / RGBLED_PWMFREQ ) - 1;

	ChannelPulse = (uint16_t) (((uint32_t) duty * (TimerPeriod - 1)) / RGBLED_PWM_MAX);

	switch (color)
	{
		case (RGBLED_RED):
				RGBLED_TIMER->CCR1 = ChannelPulse;
		break;

		case (RGBLED_GREEN):
				RGBLED_TIMER->CCR2 = ChannelPulse;
		break;

		case (RGBLED_BLUE):
				RGBLED_TIMER->CCR3 = ChannelPulse;
		break;

		default:

		break;
	}
}

void dwarf_whiteled_set_duty(uint16_t duty)
{
	/*
	dwarf_whiteled_set_duty(5000);	// 50%
	 */

	volatile uint16_t ChannelPulse = 0;
	volatile uint16_t TimerPeriod = (SYSTEMCORECLOCK / WHITELED_PWMFREQ ) - 1;

	ChannelPulse = (uint16_t) (((uint32_t) duty * (TimerPeriod - 1)) / WHITELED_PWM_MAX);
	WHITELED_TIMER->CCR4 = ChannelPulse;
}

void dwarf_fanx_init(uint8_t fan)
{
	volatile uint32_t TimerPeriod = 0;
	volatile uint16_t ChannelPulse = 0;
	uint32_t pwmfreq = FAN_PWMFREQ;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(FAN_PORT_CLOCKS, ENABLE);

	// AF PORTY VYSTUPU CASOVACE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	if (fan == FAN0)
	{
		GPIO_InitStructure.GPIO_Pin = FAN0_PIN;
		GPIO_PinAFConfig(FAN_PORT, FAN0_PIN_SOURCE, GPIO_AF_0);	// FAN0 jako TIM3_CH1
		GPIO_Init(FAN_PORT, &GPIO_InitStructure);
		GPIO_ResetBits(FAN_PORT, FAN0_PIN);						// FAN0 off
	}
	else
	if (fan == FAN1)
	{
		GPIO_InitStructure.GPIO_Pin = FAN1_PIN;
		GPIO_Init(FAN_PORT, &GPIO_InitStructure);
		GPIO_PinAFConfig(FAN_PORT, FAN1_PIN_SOURCE, GPIO_AF_0);	// FAN1 jako TIM3_CH2
		GPIO_ResetBits(FAN_PORT, FAN1_PIN);						// FAN1 off
	}

	/* Compute the value to be set in ARR regiter to generate signal frequency at pwmfreq Hz */
	TimerPeriod = (SYSTEMCORECLOCK / pwmfreq ) - 1;

	// Compute CCR1 value to generate a duty cycle at PWM_DUTY_INIT%
	ChannelPulse = (uint16_t) (((uint32_t) FAN_PWM_DUTY_INIT * (TimerPeriod - 1)) / FAN_PWM_MAX);

	/* PWM_TIM clock enable */
	RCC_APB1PeriphClockCmd(FAN_TIMER_CLOCKS, ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = FAN_TIM_PRESCALER;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(FAN_TIMER, (TIM_TimeBaseInitTypeDef *)&TIM_TimeBaseStructure);

	/* Channel Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	TIM_OCInitStructure.TIM_Pulse = ChannelPulse;

	if (fan == FAN0)
	{
		fan0_value = ChannelPulse;
		TIM_OC1Init(FAN_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
		TIM_OC1PreloadConfig(FAN_TIMER, TIM_OCPreload_Enable);
	}
	else
	if (fan == FAN1)
	{
		fan1_value = ChannelPulse;
		TIM_OC2Init(FAN_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
		TIM_OC2PreloadConfig(FAN_TIMER, TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(FAN_TIMER, ENABLE);
	TIM_SelectOutputTrigger(FAN_TIMER, TIM_TRGOSource_OC1Ref);
	TIM_Cmd(FAN_TIMER, ENABLE);
}

void dwarf_fanx_set_duty(uint8_t fan, uint16_t duty)
{
	/*
	dwarf_fanx_set_duty(5000);	// 50%
	 */

	volatile uint16_t ChannelPulse = 0;
	volatile uint16_t TimerPeriod = (SYSTEMCORECLOCK / FAN_PWMFREQ ) - 1;

	ChannelPulse = (uint16_t) (((uint32_t) duty * (TimerPeriod - 1)) / FAN_PWM_MAX);

	if (fan == FAN0)
	{
		FAN_TIMER->CCR1 = ChannelPulse;
	}
	else
	if (fan == FAN1)
	{
		FAN_TIMER->CCR2 = ChannelPulse;
	}
}

uint16_t dwarf_fanx_get_value(uint8_t fan)
{
	if (fan == FAN0)
	{
		return fan0_value;
	}
	else
	if (fan == FAN1)
	{
		return fan1_value;
	}
	return 0;
}

void dwarf_heater_init(void)
{
	volatile uint32_t TimerPeriod = 0;
	volatile uint16_t ChannelPulse = 0;
	uint32_t pwmfreq = HEATER_PWMFREQ;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(HEATER_PORT_CLOCKS, ENABLE);

	// AF PORTY VYSTUPU CASOVACE
	// Heater
	GPIO_InitStructure.GPIO_Pin = HEATER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(HEATER_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(HEATER_PORT, HEATER_PIN);				// Heater off
	GPIO_PinAFConfig(HEATER_PORT, HEATER_PIN_SOURCE, GPIO_AF_5);	// Heater jako TIM16_CH1


	  /* TIM1 Configuration ---------------------------------------------------
	   Generate 7 PWM signals with 4 different duty cycles:
	   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)
	    => TIM1CLK = PCLK2 = SystemCoreClock
	   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	   SystemCoreClock is set to 48 MHz for STM32F0xx devices

	   The objective is to generate 7 PWM signal at 17.57 KHz:
	     - TIM1_Period = (SystemCoreClock / 17570) - 1
	   The channel 1 and channel 1N duty cycle is set to 50%
	   The channel 2 and channel 2N duty cycle is set to 37.5%
	   The channel 3 and channel 3N duty cycle is set to 25%
	   The channel 4 duty cycle is set to 12.5%
	   The Timer pulse is calculated as follows:
	     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

	   Note:
	    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	    function to update SystemCoreClock variable value. Otherwise, any configuration
	    based on this variable will be incorrect.
	  ----------------------------------------------------------------------- */

	/* Compute the value to be set in ARR regiter to generate signal frequency at pwmfreq Hz */
	TimerPeriod = (SYSTEMCORECLOCK / pwmfreq ) - 1;

	/* Compute CCR4 value to generate a duty cycle at PWM_DUTY_INIT%  for channel 4 and 3N */
	ChannelPulse = (uint16_t) (((uint32_t) HEATER_PWM_DUTY_INIT * (TimerPeriod - 1)) / HEATER_PWM_MAX);

	/* PWM_TIM clock enable */
	RCC_APB2PeriphClockCmd(HEATER_TIMER_CLOCKS, ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = HEATER_TIM_PRESCALER;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(HEATER_TIMER, (TIM_TimeBaseInitTypeDef *)&TIM_TimeBaseStructure);

	/* Channel 1 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	heater_value = ChannelPulse;
	TIM_OCInitStructure.TIM_Pulse = ChannelPulse;
	TIM_OC1Init(HEATER_TIMER, (TIM_OCInitTypeDef *)&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(HEATER_TIMER, TIM_OCPreload_Enable);

	/* HEATER_TIM counter enable */
	TIM_Cmd(HEATER_TIMER, ENABLE);

	/* HEATER_TIM Main Output Enable */
	TIM_CtrlPWMOutputs(HEATER_TIMER, ENABLE);
}


void dwarf_heater_set_duty(uint16_t duty)
{
	/*
	dwarf_heater_set_duty(5000);	// 50%
	 */

	volatile uint16_t ChannelPulse = 0;
	volatile uint16_t TimerPeriod = (SYSTEMCORECLOCK / HEATER_PWMFREQ ) - 1;

	ChannelPulse = (uint16_t) (((uint32_t) duty * (TimerPeriod - 1)) / HEATER_PWM_MAX);

	HEATER_TIMER->CCR1 = ChannelPulse;
}
