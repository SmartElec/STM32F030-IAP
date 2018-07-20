#ifndef __USART_H
#define __USART_H
	 
#include "common.h"

#define UART_RX_LEN 1040

typedef struct
{
	uint8_t recv_buf[UART_RX_LEN];
	uint16_t count;
	uint8_t uart_recv_flag;

}_UART_RECEIVE;

extern _UART_RECEIVE uart_recv;


void USART1_Configuration(uint32_t bound);
void Send_String(uint8_t *pSTR);
void Usart_SendBytes(u8 array[],u16 len);
void Send_Byte(uint8_t ch);
void USART_Restart(void);
void uart_putu32(uint32_t ww);
void uart_putu16(uint16_t ww);
int uart_putc(int ch);
#endif	   
















