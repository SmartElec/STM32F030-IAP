
#include "systick.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"

iapfun jump2app; 
u16 iapbuf[1024];   
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

int iap_read_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 num=0;
	u32 fwaddr=appxaddr;//当前读取的地址
	u8 *dfu=appbuf;
	if(appsize>1024)
		return -1;
	num=appsize/2+appsize&0x10;
	STMFLASH_Read(fwaddr,iapbuf,num);
	for(t=0;t<num;t++)
	{
		dfu[t*2]=(u8)(iapbuf[t]<<8);
		if((t*2+1)>appsize)
		{
			break;
		}
		dfu[t*2+1]=(u8)iapbuf[t];
		
	}
	return 0;
}


//__set_PRIMASK(1);//关闭中断
//__set_PRIMASK(0);//打开中断
//__disable_irq(); //关闭总中断 { __ASM volatile ("cpsid i"); }
//__enable_irq(); //开启总中断 { __ASM volatile ("cpsie i"); }
void Reset_AllPeriph(void)
{
	//此处应该关闭已经打开的中断，而不是全部中断
	USART_ITConfig(USART1,USART_IT_IDLE,DISABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,DISABLE); 
	USART_Cmd(USART1, DISABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE); 
	SysTick->CTRL=0;
	
    SYSCFG_DeInit();
    RCC_DeInit();
}
//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		Reset_AllPeriph(); 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}		 

void IAP_Set(void)
{
   uint32_t i = 0;

/* Relocate by software the vector table to the internal SRAM at 0x20000000 ***/  

  /* Copy the vector table from the Flash (mapped at the base of the application
     load address 0x08003000) to the base address of the SRAM at 0x20000000. */       
  for(i = 0; i < 48; i++)
  {
    *((uint32_t*)(0x20000000 + (i << 2)))=*(__IO uint32_t*)(FLASH_APP1_ADDR + (i<<2));
	}
  /* Enable the SYSCFG peripheral clock*/ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
	
	/* Remap SRAM at 0x00000000 */
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);

}












