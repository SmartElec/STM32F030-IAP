#ifndef __LED_H
#define	__LED_H

#include "stm32f0xx.h"


void LED_Init(void);
void LED_Toggle(void);
void LED_ON(void);
void LED_OFF(void);
void LED_Flash(void* a);
#endif /* __LED_H */

