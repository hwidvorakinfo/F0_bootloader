
#include "gpio.h"
#include "application.h"

void irq_init(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	// PANIC
	SYSCFG_EXTILineConfig(PANIC_EXTI_SOURCE, PANIC_EXTI_PIN);			// Connect EXTI Line to PANICPIN pin
	EXTI_InitStructure.EXTI_Line = PANIC_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = PANIC_TRIGGER;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);										// Configure EXTIx line
	NVIC_InitStructure.NVIC_IRQChannel = PANIC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = PANIC_PRIORITY;		// the higher number the lower priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);										// Enable and set EXTIx Interrupt

	// BUTTON0
	SYSCFG_EXTILineConfig(BUTTON0_EXTI_SOURCE, BUTTON0_EXTI_PIN);		// Connect EXTI Line to BUTTON0 pin
	EXTI_InitStructure.EXTI_Line = BUTTON0_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);										// Configure EXTIx line
	NVIC_InitStructure.NVIC_IRQChannel = BUTTON0_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = BUTTON0_PRIORITY;		// the higher number the lower priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);										// Enable and set EXTIx Interrupt

	// BUTTON1
	SYSCFG_EXTILineConfig(BUTTON1_EXTI_SOURCE, BUTTON1_EXTI_PIN);		// Connect EXTI Line to BUTTON1 pin
	EXTI_InitStructure.EXTI_Line = BUTTON1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);										// Configure EXTIx line
	NVIC_InitStructure.NVIC_IRQChannel = BUTTON1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = BUTTON1_PRIORITY;		// the higher number the lower priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);										// Enable and set EXTIx Interrupt

	// PICKED0
	SYSCFG_EXTILineConfig(PICKED0_EXTI_SOURCE, PICKED0_EXTI_PIN);		// Connect EXTI Line to PICKED0 pin
	EXTI_InitStructure.EXTI_Line = PICKED0_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);										// Configure EXTIx line
	NVIC_InitStructure.NVIC_IRQChannel = PICKED0_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = PICKED0_PRIORITY;		// the higher number the lower priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);										// Enable and set EXTIx Interrupt

	// PICKED1
	SYSCFG_EXTILineConfig(PICKED1_EXTI_SOURCE, PICKED1_EXTI_PIN);		// Connect EXTI Line to PICKED1 pin
	EXTI_InitStructure.EXTI_Line = PICKED1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);										// Configure EXTIx line
	NVIC_InitStructure.NVIC_IRQChannel = PICKED1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = PICKED1_PRIORITY;		// the higher number the lower priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);										// Enable and set EXTIx Interrupt
}
