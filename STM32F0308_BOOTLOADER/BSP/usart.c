#include "systick.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{  
	while(!((USART1->ISR)&(1<<7))){}
  USART1->TDR=ch;
  return (ch);
}
#endif 



static uint8_t USART_RX_BUF[UART_RX_LEN]; 
 _UART_RECEIVE uart_recv;


void USART1_IRQHandler(void) 
{
    uint16_t Len = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
    {  
        USART1->ICR |= 1<<4; //清除中断   
        
        DMA_Cmd(DMA1_Channel3,DISABLE);  
        Len = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);  
		
        if((Len >0)&&(Len < UART_RX_LEN)&&(uart_recv.uart_recv_flag==0))
		{
			memcpy(uart_recv.recv_buf, USART_RX_BUF, Len);
			uart_recv.count = Len;
			uart_recv.recv_buf[uart_recv.count]=0;
			uart_recv.uart_recv_flag = 1;
        }
	
	//设置传输数据长度  
	DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
	//打开DMA  
	DMA_Cmd(DMA1_Channel3,ENABLE);
    }
}

static void USART1_DMA_Config()
{
    DMA_InitTypeDef DMA_InitStructure;  
  
    //串口收DMA配置    
    //启动DMA时钟  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1通道5配置  
    DMA_DeInit(DMA1_Channel3);  
    //外设地址  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->RDR);  
    //内存地址  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART_RX_BUF;  
    //dma传输方向单向  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;  
    //设置DMA的外设递增模式，一个外设  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //外设数据字长  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //内存数据字长  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //设置DMA的传输模式  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA_Mode_Circular DMA_Mode_Normal
    //设置DMA的优先级别  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
    //设置DMA的2个memory中的变量互相访问  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel3,&DMA_InitStructure);  
  
    //使能通道3  
    DMA_Cmd(DMA1_Channel3,ENABLE);
}

//初始化IO 串口1
void USART1_Configuration(uint32_t bound)
{  	 
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//USART_WordLength_8b;//此处在有奇偶校验时需要9个数据位，无奇偶校验时8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;//USART_Parity_No;	//偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);//初始化串口


	USART1_DMA_Config();

	//TXE发送中断,TC传输完成中断,RXNE接收中断,PE奇偶错误中断,可以是多个       
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);    
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART1->ICR |= 1<<4;     //必须先清除IDLE中断，否则会一直进IDLE中断
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	//采用DMA方式发送  
	//USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
	//采用DMA方式接收  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 

	USART_Cmd(USART1, ENABLE);
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=0 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

	uart_recv.uart_recv_flag = 0;
}
void USART_Restart(void)
{
	uart_recv.uart_recv_flag = 0;

}
int uart_putc(int ch)
{
while(!((USART1->ISR)&(1<<7))){}
	USART1->TDR=ch;
	return ch;
}
void uart_putu16(uint16_t ww) {
  uart_putc(ww >>  0 & 0x000000ff);
  uart_putc(ww >>  8 & 0x000000ff);
}
void uart_putu32(uint32_t ww) {
  uart_putc(ww >>  0 & 0x000000ff);
  uart_putc(ww >>  8 & 0x000000ff);
  uart_putc(ww >> 16 & 0x000000ff);
  uart_putc(ww >> 24 & 0x000000ff);
}
void Send_String(uint8_t *pSTR)
{				
	while(*pSTR)
	{
		while(!((USART1->ISR)&(1<<7))){}
		USART1->TDR=*pSTR++;
	}
}
void Usart_SendBytes(u8 array[],u16 len)
{
	u8 i=0;
	while(len)
	{
		while((USART1->ISR&0X80)==0){}
			USART1->TDR = array[i++]; 
		len--;
	}		
}
void Send_Byte(uint8_t ch)
{				
	while(!((USART1->ISR)&(1<<7))){}
	USART1->TDR=ch;
}


