#include "Key.h"                  // Device header
#include "main.h" 

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

//传入结构体指针变量 
uint8_t Key_GetNum(KEY_CB * KEY)      //获取中断中的消抖后的电平状态
{
	return  KEY->stable_statue;

}

void Key_Process(KEY_CB * KEY)      //消抖
{
	uint8_t old_stable= KEY->stable_statue;
	
	KEY->now_statue = GPIO_ReadInputDataBit(GPIOB,KEY->pin);
	
	/*20ms消抖消抖*/
	if(KEY->now_statue == KEY->last_statue)
		{ 
 			 
			 KEY->deb_cnt ++;
				 if(KEY->deb_cnt == 2 )      
				 {  
					 
					KEY->deb_cnt =0;
					KEY->stable_statue = KEY->now_statue;	 
				 
				 }
				 
		}
		
		else 
		{
			 KEY->deb_cnt = 0;
			 KEY->last_statue = KEY->now_statue; 
		}
	
		
	 	/*识别长按和短按*/
		/* ========== 以下三段每拍都跑,只看稳定值 ========== */
		if(old_stable == 1 && KEY->stable_statue ==0 )                //从松开到按下
		{
		  	KEY->press_cnt = 0;
					
		}
		if(KEY->stable_statue == 0)                                 //按住中才计时,长按
		{
			KEY->press_cnt++;
			if(KEY->press_cnt == 100)
			{
				
				KEYStaFlag |= KEY->evt_long;
			}
		}
			
		 if(old_stable == 0 && KEY->stable_statue == 1)                  //松开时候记录press_cnt识别短按
		 {
			 if(KEY->press_cnt<100)
			 {
				 KEYStaFlag |= KEY->evt_short;
			 }
			 
		 }

}



