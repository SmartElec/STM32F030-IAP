#ifndef __CRC8_16_H
#define __CRC8_16_H
#include<stm32f0xx.h>

uint16_t CRC16RTU( uint8_t * pszBuf, uint16_t unLength);
uint16_t Get_Crc16(uint8_t *puchMsg,uint16_t usDataLen);
uint8_t Get_Crc8(uint8_t *ptr,uint16_t len);
#endif




























