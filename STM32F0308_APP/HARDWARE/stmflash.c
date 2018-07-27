#include "common.h"
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
u16 STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		if(FLASH_COMPLETE!=FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]))
		{
			return 1;
		}
	    WriteAddr+=2;//地址增加2.
	}  
	return 0;
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节

u16 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return 1;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			if(FLASH_COMPLETE!=FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE))//擦除这个扇区
			{
				FLASH_Lock();//上锁
				return 1;
			}
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];
			}
			
			if(FLASH_COMPLETE!=STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2))//写入整个扇区  
			{
				FLASH_Lock();//上锁
				return 1;
			}
		}else 
		{
			if(FLASH_COMPLETE!=STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain))//写已经擦除了的,直接写入扇区剩余区间. 		
			{
				FLASH_Lock();//上锁
				return 1;
			}
		}
				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
	return 0;
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}

//用户选项字节
//u16  OptionByte;
u16 OptionByte_Read(void)      //两个8字节合成1个16字节使用
{
	u8 high=(*(vu16*)(0x1FFFF804))&0x00ff;//读0x1FFFF804
	u8 low =(*(vu16*)(0x1FFFF806))&0x00ff;//读0x1FFFF806
	return (   (high<<8)+low  );
}
void OptionByte_Write(u16 data)//两个8字节合成1个16字节使用
{
	u16  OptionByte;
	OptionByte=OptionByte_Read();
	if(OptionByte==data)
		return;
	FLASH_Unlock();//解锁
	FLASH_OB_Unlock();
	if(OptionByte!=0xffff)
		FLASH_EraseOptionBytes();//flash为ff时候才能写入，所以先擦除，网上很多不成功就是没擦除
	if(data!=0xffff)
	{
		FLASH_ProgramOptionByteData(0x1FFFF804,data>>8);    //写0x1FFFF804
		FLASH_ProgramOptionByteData(0x1FFFF806,data&0x00ff);//写0x1FFFF806
	}
	FLASH_OB_Lock();
	FLASH_Lock();  //上锁
	
}













