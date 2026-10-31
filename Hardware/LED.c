#include "LED.h"                  // Device header

void LED_Init(void)
{  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//LED接PC13
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //输出模式,F103输出无法上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);            //使能GPIOC
	GPIO_SetBits(GPIOC,GPIO_Pin_13 );                 //PC13输出高电平不导通,小灯熄灭
}

void LED_ON(void)                                  //此板上LED高电平熄灭SetBits,低电平亮ResetBits
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);              //输出低电平,导通led亮
}

void LED_OFF(void)                            
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);                //输出高电平,不导通led灭
}

void LED_Turn(void)                                 //把 LED 的当前状态变成相反的状态
{
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0)    //默认低电平为0,高电平为1
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}
