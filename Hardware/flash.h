#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"  
#include <stdint.h>
#include "usart.h"

#define STM32_FLASH_SADDR         0x08000000          //FLASH起始地址
#define STM32_PAGE_SIZE           1024                //FLASH页大小
#define STM32_PAGE_NUM            64                  //FLASH总页数
#define STM32_B_PAGE_NUM          36                  //B总页数
#define STM32_A_PAGE_NUM         (STM32_PAGE_NUM - STM32_B_PAGE_NUM)   //A总页数
#define STM32_A_START_PAGE        STM32_B_PAGE_NUM                     //A起始页
#define STM32_A_SADDR            (STM32_FLASH_SADDR + STM32_A_START_PAGE * STM32_PAGE_SIZE) //A起始地址



#define APP_BIN_SIZE   3466




void STMF103_32_Erase (uint16_t start, uint16_t num);
void STMF103_32_Write(uint32_t Start_addr, uint32_t * wdata,uint32_t wnum);

typedef void (*load_a)(void);       //函数指针,指向函数,调用指针=调用函数,改变PC

__asm void MSR_SP(uint32_t addr);

void LOAD_A(uint32_t addr);
#endif



