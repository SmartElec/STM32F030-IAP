#ifndef __HMI_UART_H
#define	__HMI_UART_H

#include "stm32f0xx.h"

//播放[暂停]/停止/快进+/快退-/上一曲/下一曲/音量+/音量-/静音/
typedef enum
{
	CMD_PLAY_PAUSE=0,
	CMD_STOP=1,
	CMD_FAST=2,
	CMD_FORWARD=3,
	CMD_PREV=4,
	CMD_NEXT=5,
	CMD_MUTE=6,
	CMD_VOL_U=7,
	CMD_VOL_D=8,
	CMD_GET_VOL=0xF0,
	CMD_GET_DETAIL=0xF1,
}KEY_CODE_TYPE;

extern KEY_CODE_TYPE KEY_CODE;

void HMI_Analysis(unsigned char* datas);
void OPERATE_IR_KEY(uint8_t id,uint8_t event);
void HMI_init(void);
#endif /* __HMI_UART_H */
