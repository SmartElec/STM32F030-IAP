//64K FLASH   8K RAM
/* Includes -------------------*/
#include "common.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
//#include "iap.h"
#include "crc8_16.h"
#include "softtimer.h"
//#include "stmflash.h"
/****************************************************************************************************/
#define BOOT_Version  	0x0220
#define FLASH_ROM   	64
#define FILE_SIZE_A  	*(vu32*)APP1_SIZE_ADDR//用于存放app的大小
#define SPLIT_PACK 		256

char* filename_ptr="firmware.bin";
char file_name[50];

u32 FILE_SIZE=0;
u32 file_size=0;
u32 packet_size=0;
u32 packets_all=0;
u32 packets_now=0;

u8 machine_state=0;
ifs_stmr_handle statemachine_handle;
ifs_stmr_handle led_handle=-1;
ifs_stmr_handle IAP_handle;

void Send_File(void);
void state_timeout(void *arg);
int Wait_Ack(u8 ack_ch);

typedef void (*FunType)(void);
FunType Fun_IAPSet= (void(*)())0x8001001;
FunType Fun_ReadSuccessflag= (void(*)())0x8001031;
FunType Fun_WriteSuccessflag= (void(*)())0x8001061;

int main(void)
{
	u8 count=0;
//	u16 temp_addr[2];
	u16 crc_temp,calc_crc;
	
	(*Fun_IAPSet)();//调用IAPSet()函数 在bootloader中
	Systick_Init();
	LED_Init();
	USART1_Configuration(115200);
	ifs_stmr_init();

	ifs_stmr_start(ifs_stmr_registered(200,LED_Flash,NULL,IFS_STMR_FLAG_PERIOD));

	printf("-APP-总空间:%ld Byte ",FLASH_ROM*1024ul);
	uart_putu16(0xffff);
	while(1)
	{
		if(uart_recv.uart_recv_flag)
		{
			switch(machine_state)
			{
				case 0:
					if((uart_recv.recv_buf[0]==0xaa)&&(uart_recv.recv_buf[1]==0xbb)&&(uart_recv.recv_buf[2]==0xcc)&&(uart_recv.recv_buf[3]==0xdd))
					{//4+4+2+...
						//u16 calc_crc=0,crc_temp=0;
						crc_temp=(uart_recv.recv_buf[10]<<8)|(uart_recv.recv_buf[11]);
						calc_crc=Get_Crc16(&uart_recv.recv_buf[0]+4,6);
						calc_crc=(calc_crc<<8)|(calc_crc>>8);
						if(calc_crc==crc_temp)
						{
							file_size=(uart_recv.recv_buf[4]<<24);
							file_size|=(uart_recv.recv_buf[5]<<16);
							file_size|=(uart_recv.recv_buf[6]<<8);
							file_size|=(uart_recv.recv_buf[7]);
							
							FILE_SIZE=file_size;
							//(FLASH_APP1_ADDR-0X08000000)//APP不能使用的空间 保留为BOOTLOADER使用
							if(FILE_SIZE>(FLASH_ROM*1024-((uint32_t)0x08003000-0X08000000)))
							{
								uart_putu16(0xffff);
								break;
							}
							packet_size=uart_recv.recv_buf[8]<<8|uart_recv.recv_buf[9];
							packets_all=file_size/packet_size+(file_size%packet_size?1:0);
							
							packets_now=0;

							strncpy(file_name,(const char*)(&uart_recv.recv_buf[0]+12),100);
							//filename_ptr=file_name;
		
							ifs_stmr_start(statemachine_handle);
							//调用写成功标志的函数 再bootloader中
							(*Fun_WriteSuccessflag)();
							uart_putc(0x06);
						}
						else
						{
							uart_putc(0x07);
						}
					}
					else if((uart_recv.recv_buf[0]==0xaa)&&(uart_recv.recv_buf[1]==0xbb)&&(uart_recv.recv_buf[2]==0xee))
					{
						NVIC_SystemReset();//系统复位
					}
					else
					{
						uart_putu16(0xffff);
					}
					break;

				default:
					break;
			}
			USART_Restart();
		}

		count++;
		delay_ms(10);
		if(count>200)
		{
			count=0;
		}
		ifs_stmr_task();
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
