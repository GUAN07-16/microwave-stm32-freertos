#include "stm32f10x.h"                  // Device header
#include "main.h" 
#include "LED.h"  
#include "Key.h" 
#include "Timer.h"
#include "PWM.h"  
#include "SPI.h"
#include "GUI.h"
#include "lcd.h"
#include "dht11.h"
#include "beep.h"

KEY_CB KEY1={GPIO_Pin_12,KEY1_SHORT_FLAG,KEY1_LONG_FLAG,0,0,0,1,1};
KEY_CB KEY2={GPIO_Pin_1,KEY2_SHORT_FLAG,KEY2_LONG_FLAG,0,0,0,1,1};
DHT_CB DHT11 ={ GPIOC ,GPIO_Pin_14}; //GPIOC就是指针


volatile uint32_t KEYStaFlag = 0;    //位掩码,一个变量里同时存放多个独立的状态标志,值判断对应位
uint8_t Door_state =0;
uint8_t power_cnt = 2;
volatile uint8_t time =0;
volatile uint8_t heating =0;
volatile uint8_t complete =0;            
static uint8_t time_shown = 0xFF;    /* 0xFF:255开机第一次比较必不相等”(强制首刷) */
uint8_t temp = 0;
uint8_t humidity = 0;
uint8_t tick = 0;


int main()
{ 
	LED_Init();
	Timer_Init();
	TIM4_Init();
	Key_Init();
	TIM1_PWM_Init();
	TIM2_PWM_Init();
	LCD_Init();
	beep_Init();     //beep()里面两个Delay_ms(600),调用放后面不影响前面加热启动
	
	Show_Str(0, 60, BLUE, WHITE, "Microwave V1.1", 16, 0);
	Delay_ms(2000);
	LCD_Clear(WHITE);    //全屏填充白色，清屏函数
	
  Door_Open(); 
	Door_state = 1;
	LED_ON();
	ui_door("OPEN");            /* Door:OPEN */
	ui_power("MID");            /* Power: MID */
	ui_time(0);                 /* Time: 0 */
	
	
	
	Delay_ms(1500);
	if(DHT_Read_Data(&temp,&humidity,GPIOC,GPIO_Pin_14,&DHT11)==1)
	{
			ui_DHT11_temp(temp);
			ui_DHT11_humidity(humidity);		
	}
	
	
	
	while(1)
	{	
		
		
		if(time_shown != time)    /* 数据和屏上不一致才刷,一致跳过 */
		{
			  
				time_shown = time;
				ui_time(time);
			  tick++;
			
			  if(tick==2 )           //2s刷新一次温度和
				{
					tick =0;   //无条件消费,不管下面成败,节流计时重新开始 
					if(DHT_Read_Data(&temp,&humidity,GPIOC,GPIO_Pin_14,&DHT11)==1)
					{
						ui_DHT11_temp(temp);
						ui_DHT11_humidity(humidity);		
					}
				}	
		}
		
		
		
		
		if(complete == 1)            //加热完成
		{
			complete = 0;              //清标志
			heating =0;
			Door_Open(); 
			Door_state = 1;
			LED_ON();
			Turntable_Stop();	
			Heater_Set(0);	
			ui_door("OPEN"); 
			ui_hint("Heating complete");
			//加热完成恢复默认挡位
			power_cnt = 2; 
			ui_power("MID");
		  beep(3);
			
		}
		
		if(complete ==0 && heating ==1 && Door_state == 1)   //加热中开门
		{
			heating = 0;
			LED_ON();  
			Turntable_Stop();	
			Heater_Set(0);
			ui_hint("Heating suspend");
			TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);   
		  TIM_Cmd(TIM4, DISABLE);
		 
		}
		
		
		if(heating==1 && temp >= 30)                         //加热中温度过高
		{
			heating = 0;
			LED_ON();  
			Door_Open(); 
			Door_state=1;
			Turntable_Stop();	
			Heater_Set(0);
			ui_hint("Over heat!");
			TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);   
		  TIM_Cmd(TIM4, DISABLE);		
			beep(4);
		}
		
		
		
		
		
		
		
		//开门灯亮,关门灯灭
		
		if(KEYStaFlag & KEY1_SHORT_FLAG)                          //按键1短按切换门状态
		{
			KEYStaFlag = 0; 
			
			if(Door_state ==0)                                      //关门切换为开门(灯亮)
			{
				Door_Open();
				Door_state=1;//切换为关开门即=1
				LED_ON();
				ui_door("OPEN");   
			
			}
			else if (Door_state ==1)                                //开门切换为关门(灯灭)
			{
				Door_Close();
				Door_state = 0; 
				LED_OFF();
			  Heater_Set(0);
				ui_door("CLOSED");   
			
			}
			
		}

		//改无条件消费标志位,防卡死
		if(KEYStaFlag & KEY1_LONG_FLAG)          //按键1长按启动加热,使能中断,中断服务函数只用于计时
		{
			KEYStaFlag = 0;
		
			if(time==0)
			{
			 ui_hint("increase time");
			}
			else    //time>0
			{
					if(Door_state==0)           //关门
					{					
						ui_hint("heating");
						heating	=1;		
						Turntable_Start();	                                                    //进中断有1s延迟所以先进行启动
						Heater_Set(power_cnt==1 ? HEATER_LOW : power_cnt==2 ? HEATER_MED : HEATER_HIGH);
						TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);   
						TIM_Cmd(TIM4, ENABLE);
						beep(2);               //beep()里面两个Delay_ms(600),调用放后面不影响前面加热启动
					}
					else if(Door_state!=0)       //开门
					{
						ui_hint("wait close");
            beep(1);					
					}				
			}
			
			
		}
		
		
		if(KEYStaFlag & KEY2_SHORT_FLAG)       //按键2短按增加加入时长
		{
			KEYStaFlag = 0; 
			time+=20;
			if(time>200)
			{
				time = 0;
			}
				
		}
		if((KEYStaFlag & KEY2_LONG_FLAG))   //按键2长按切换加热挡位,只有不在加热的时候，才能改档位
		{
			KEYStaFlag = 0; 
			power_cnt = power_cnt % 3 + 1;  
		  ui_power(power_cnt==1 ? "LOW" : power_cnt==2 ? "MID" : "HIGH");
		}



	}
	
	
	
}



	

