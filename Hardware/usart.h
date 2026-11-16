#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define U1_RxBuff_addr  0x20004000

void Usart1_DMA_Init(uint32_t bandrate,uint16_t Num);
void u1_printf(char *format, ...);

#endif


