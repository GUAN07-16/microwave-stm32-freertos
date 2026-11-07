#include "stm32f10x.h"                  // Device header
#include "main.h" 
#include "LED.h"  
#include "Key.h" 
#include "Timer.h"
#include "PWM.h"  

KEY_CB KEY1={GPIO_Pin_12,KEY1_SHORT_FLAG,KEY1_LONG_FLAG,0,0,0,1,1};
KEY_CB KEY2={GPIO_Pin_1,KEY2_SHORT_FLAG,KEY2_LONG_FLAG,0,0,0,1,1};

volatile uint32_t KEYStaFlag = 0;    //位掩码,一个变量里同时存放多个独立的状态标志,值判断对应位

int main()
{ 
	LED_Init();
	Timer_Init();
	Key_Init();
	TIM1_PWM_Init();
	TIM2_PWM_Init();
	while(1)
	{
		
		if(KEYStaFlag & KEY1_LONG_FLAG)   //长按关门
		{
			KEYStaFlag = 0; 
			LED_ON();
  		Door_Open();                     
		}

		if(KEYStaFlag & KEY1_SHORT_FLAG)   //短按开门
		{
			KEYStaFlag = 0; 
			LED_OFF();                  
			Door_Close();                    
		}
		
		if(KEYStaFlag & KEY2_LONG_FLAG)    //长按转动
		{
			KEYStaFlag = 0; 
			LED_ON();
			Turntable_Start();
						
		}

		if(KEYStaFlag & KEY2_SHORT_FLAG)   //短按停止
		{
			KEYStaFlag = 0; 
			LED_OFF();
			Turntable_Stop();			
		}
	

	}
	
	
	
}



	