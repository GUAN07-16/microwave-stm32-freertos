#include "Key.h"                  // Device header


volatile uint8_t stable_statue = 1;
//static → 就是你说的“只作用于本文件”，别的 .c 文件看不见它，不会重名。
//volatile → 中断里写、主循环里读，不加的话编译器可能认为主循环里没人改它，把判断优化掉。

//PB12-SW1,PB1
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                //模块自带的外部上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
uint8_t Key_GetNum(void)    //获取中断中的消抖后的电平状态
{
	return  stable_statue;
	
}