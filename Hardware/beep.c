#include "beep.h"

void beep_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //使能GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_15 );                 //输出1,不响
	
	//SeBit  ->1
}


//该蜂鸣器低电平触发
/*beep(uint8_t n):n 次“响 600ms → 停 600ms”*/

void beep(uint8_t n)
{
	uint8_t i;
	for(i=0;i<n;i++)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);                //输出0,响
		Delay_ms(600);
		GPIO_SetBits(GPIOB,GPIO_Pin_15);                  //输出1,不响
		if(i!=n-1)                                       
		{                      
			Delay_ms(600);           //最后一次响600停600中,停600消去
		}
	}	
}
