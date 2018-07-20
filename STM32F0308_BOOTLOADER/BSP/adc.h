#ifndef __ADC_H
#define	__ADC_H

#include "stm32f0xx.h"
		 
extern	__IO uint32_t TempSensVoltmv, VrefIntVoltmv;
extern	__IO uint16_t RegularConvData_Tab[2];

void ADC1_CH_DMA_Config(void);

#endif /* __ADC_H */



