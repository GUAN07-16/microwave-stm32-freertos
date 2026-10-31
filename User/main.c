#include "stm32f10x.h"                  // Device header
#include "LED.h"  
#include "Key.h" 
#include "Timer.h"



int main()
{ 
	LED_Init();
	Timer_Init();
	Key_Init();
	while(1)
	{
		if(Key_GetNum() == 1)
		{
			 LED_ON();
		}
		else
		{
			 LED_OFF();
		}
	}
}











	