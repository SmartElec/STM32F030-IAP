#include "led.h"
#include "common.h"

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RUN_LED_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RUN_LED_GPIO_Port, &GPIO_InitStructure);
	RUN_LED_GPIO_Port->BSRR = RUN_LED_Pin;
}

void LED_Toggle(void)
{
	RUN_LED_GPIO_Port->ODR ^= RUN_LED_Pin;
}
void LED_ON(void)
{
	
	RUN_LED_GPIO_Port->BRR = RUN_LED_Pin;
}
void LED_OFF(void)
{
	RUN_LED_GPIO_Port->BSRR = RUN_LED_Pin;
}
void LED_Flash(void *a)
{
	UNUSED(a);
	RUN_LED_GPIO_Port->ODR ^= RUN_LED_Pin;
}
