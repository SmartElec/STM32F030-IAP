#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stm32f0xx.h"

///////////////////64k flash, 8k sram///////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//PAGE63  0x0800 7C00
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
u16 STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
u16 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
u16 OptionByte_Read(void) __attribute((section(".ARM.__at_0x8001030")));
void OptionByte_Write(u16 data);// __attribute((section(".ARM.__at_0x8001060")));
void OptionByte_Write_A5A5(void) __attribute((section(".ARM.__at_0x8001060")));
//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);
#endif

















