#include "stm32f10x.h"                  // Device header
#include "main.h" 
#include "LED.h"  
#include "Key.h" 
#include "Timer.h"

KEY_CB KEY1={GPIO_Pin_12,KEY1_SHORT_FLAG,KEY1_LONG_FLAG,0,0,0,1,1};
KEY_CB KEY2={GPIO_Pin_1,KEY2_SHORT_FLAG,KEY2_LONG_FLAG,0,0,0,1,1};

volatile uint32_t KEYStaFlag;    //位掩码,一个变量里同时存放多个独立的状态标志,值判断对应位

int main()
{ 
	LED_Init();
	Timer_Init();
	Key_Init();
	while(1)
	{
		if(KEYStaFlag & KEY1_LONG_FLAG) 
		{
			LED_ON();
			KEYStaFlag = 0; 
		}

		if(KEYStaFlag & KEY1_SHORT_FLAG) 
		{
			LED_OFF();
			KEYStaFlag = 0; 
		}
		
		if(KEYStaFlag & KEY2_LONG_FLAG) 
		{
			LED_ON();
			KEYStaFlag = 0; 
		}

		if(KEYStaFlag & KEY2_SHORT_FLAG) 
		{
			LED_OFF();
			KEYStaFlag = 0; 
		}
	

	}
	
	
	
}



	