#include "systick.h"
#include "softtimer.h"

static __IO uint32_t TimingDelay;

void delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
  ifs_stmr_interrupt();
}

void Systick_Init(void)
{
  if (SysTick_Config(SystemCoreClock / 1000))//1ms
  { 
    /* Capture error */ 
    while (1);
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
RCC_ClocksTypeDef RCC_Clocks;
void SystemClock_Config(void)
{
/*
	RCC_DeInit();
	RCC_HSICmd(DISABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	if(RCC_WaitForHSEStartUp()!=SUCCESS)
	{
		//_Error_Handler(__FILE__, __LINE__);
	}
	RCC_PLLConfig(RCC_PLLSource_PREDIV1,RCC_PLLMul_6);
	RCC_PLLCmd(ENABLE);
	RCC_PREDIV1Config(RCC_PREDIV1_Div1);
	RCC_ClockSecuritySystemCmd(ENABLE);
	//RCC_GetClocksFreq();
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//Configures the AHB clock (HCLK).
	RCC_PCLKConfig(RCC_HCLK_Div1);//Configures the APB clock (PCLK).	
	*/

	RCC_GetClocksFreq(&RCC_Clocks);
	__nop();
}
