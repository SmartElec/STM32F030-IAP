#ifndef __IAP_H__
#define __IAP_H__

#include<stm32f0xx.h>

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
#define FLASH_APP1_ADDR		 ((uint32_t)0x08003000)  	//第一个应用程序起始地址(存放在FLASH)
											//保留0X08000000~0X0800FFFF的空间为IAP使用
#define APP1_SIZE_ADDR		(FLASH_APP1_ADDR-4) 
#define APP1_SUCCESS_ADDR	(FLASH_APP1_ADDR-6)

void iap_load_app(u32 appxaddr);			//执行flash里面的app程序
void iap_load_appsram(u32 appxaddr);		//执行sram里面的app程序
u16 iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//在指定地址开始,写入bin
int iap_read_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);
void IAP_Set(void) __attribute((section(".ARM.__at_0x8001000")));

#endif


/*********************************************************************************************/
/*
查看FLASH跟SRAM的占用空间问题主要看下面的语句。

Program Size: Code=29864 RO-data=123592 RW-data=60 ZI-data=3900
*/
/*********************************************************************************************/
/*
其中 要着重注意的是Code、RO-data、RW-data、ZI-data所代表的意思
Code：表示程序所占用 FLASH 的大小（ FLASH）。
RO-data：即 Read Only-data， 表示程序定义的常量，如 const 类型（ FLASH）。
RW-data：即 Read Write-data， 表示已被初始化的全局变量（ SRAM）

ZI-data：即 Zero Init-data， 表示未被初始化的全局变量(SRAM)
*/
/*********************************************************************************************/
/*
那么FLASH和SRAM占用情况的计算方式如下所示：

flash = Code + RO-data + RW-data = 29864 + 123592 + 60 = 153516 bytes

sram = RW-data + ZI-data = 60+ 3900= 3960bytes

还有一个要特别注意的是程序的大小不是.hex文件的大小，而是编译结果的Code+RO-data。

在程序初始化的时候，RW-data会从FLASH中拷贝到RAM中。
*/



































