#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f0xx.h"

void SystemClock_Config(void);
void Systick_Init(void);
void delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);



#endif /* __SYSTICK_H */
