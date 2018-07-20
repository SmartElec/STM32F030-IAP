#ifndef __COMMON_H__
#define __COMMON_H__

#include<stm32f0xx.h>
#include<stdio.h>
#include "systick.h"
#include "led.h"
#include "usart.h"
#include "gpio.h"
#include "stmflash.h"
#include "softtimer.h"

#define RUN_LED_Pin GPIO_Pin_8
#define RUN_LED_GPIO_Port GPIOA


#define UNUSED(a) (void)(a)
#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif
