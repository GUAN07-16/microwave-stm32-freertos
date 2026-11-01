#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"  
void Key_Init(void);

typedef struct 
{
	uint16_t pin;                 // 我在哪个脚 , 输入GPIO_Pin_12如果使用uint8_t会被截断
  uint32_t evt_short;           // 我的短按事件占哪个位
  uint32_t evt_long;            // 我的长按事件占哪个位
	
  uint8_t deb_cnt;             //消抖计数->20ms
	uint16_t press_cnt;          //长按,短按计数
	uint8_t now_statue;
  uint8_t last_statue;       //按键对应io口初始未按下时都是高电平
  volatile uint8_t stable_statue;//volatile → 中断里写、主循环里读，不加的话编译器可能认为主循环里没人改它，把判断优化掉。
  
}KEY_CB;

uint8_t Key_GetNum(KEY_CB * KEY);
void Key_Process(KEY_CB * KEY);

#endif
