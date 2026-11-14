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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

KEY_CB KEY1={GPIO_Pin_12,KEY1_SHORT_FLAG,KEY1_LONG_FLAG,0,0,0,1,1};
KEY_CB KEY2={GPIO_Pin_1,KEY2_SHORT_FLAG,KEY2_LONG_FLAG,0,0,0,1,1};
DHT_CB DHT11 ={ GPIOC ,GPIO_Pin_14};  //GPIOC就是指针


volatile uint32_t KEYStaFlag = 0;    //位掩码,一个变量里同时存放多个独立的状态标志,值判断对应位
uint8_t Door_state =0;
uint8_t power_cnt = 2;
volatile uint8_t time =0;
volatile uint8_t heating =0;      
static uint8_t time_shown = 0xFF;    /* 0xFF:255开机第一次比较必不相等”(强制首刷) */
uint8_t temp = 0;
uint8_t humidity = 0;

TaskHandle_t keyTask;
TaskHandle_t cookingTask;
TaskHandle_t dhtTask;
SemaphoreHandle_t heating_sem;   //信号量句柄

void Task_KEY(void *params)
{
	while(1)
	{
		if(time_shown != time)    /* 数据和屏上不一致才刷,一致跳过 */
		{
			time_shown = time;
			ui_time(time);
		}
		//开门灯亮,关门灯灭
		
		if(KEYStaFlag & KEY1_SHORT_FLAG)                          //按键1短按切换门状态
		{
			KEYStaFlag &=~ KEY1_SHORT_FLAG; 
			
			if(Door_state ==0)                                      //关门切换为开门(灯亮)
		  {
			 
				Door_Open();
				Door_state=1;//切换为关开门即=1
				LED_ON();
				ui_door("OPEN"); 
				if(heating ==1 )   //加热中开门
				{
					TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);   
				  TIM_Cmd(TIM4, DISABLE);
					heating = 0; 
					Turntable_Stop();	
					Heater_Set(0);
					ui_hint("Heating suspend");
					
		    }  
			
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
			KEYStaFlag &=~ KEY1_LONG_FLAG;
		  
			if(time==0)
			{
			 ui_hint("increase time");
			}
			else    //time>0
			{
					if(Door_state==0)           //关门
					{	
            TIM_ClearFlag(TIM4, TIM_FLAG_Update);   /* 清掉上轮 disable 瞬间可能残留的标志 */						
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
			KEYStaFlag &=~ KEY2_SHORT_FLAG; 
			time+=20;
			if(time>200)
			{
				time = 0;
			}
				
		}
		if((KEYStaFlag & KEY2_LONG_FLAG))   //按键2长按切换加热挡位,只有不在加热的时候，才能改档位
		{
			KEYStaFlag &=~ KEY2_LONG_FLAG; 
			power_cnt = power_cnt % 3 + 1;  
		  ui_power(power_cnt==1 ? "LOW" : power_cnt==2 ? "MID" : "HIGH");
		}
   
		 vTaskDelay(10);            //进入阻塞态
		
	}
	
}


void cooking_Task(void *params)
{
	while(1)
	{
		//信号量只能拿一次,中断每隔1s释放信号量代表定时1s到了
	  xSemaphoreTake(heating_sem, portMAX_DELAY);         //信号量若获取成功,信号量自动清零    
    if(time>0)
		{			
	  	time--;
		}
		
		//time--变成0之后需要进入下面的的程序不能使用else if
		if (time==0 && heating==1)	     //加热完成        
		{
				TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);   
				TIM_Cmd(TIM4, DISABLE);
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
		
	}
	
}


void dht_Task(void *params)
{
	while(1)
	{
		if(DHT_Read_Data(&temp,&humidity,GPIOC,GPIO_Pin_14,&DHT11)==1)
		{
			ui_DHT11_temp(temp);
			ui_DHT11_humidity(humidity);		
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
		
		vTaskDelay(2000);                 //进入阻塞态
	
	}
	
}



int main()
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);        //所有定时器只需要分组配置一次
	
	Delay_Init();
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
	
	

	
	xTaskCreate(
        &Task_KEY,          // 任务函数地址
        "DemoTask",         // 任务名称，调试看，字符串随便写
        256,                // 栈深度！单位是word（4字节），128=512字节
        NULL,               // 传入任务的参数，不用就NULL
        4,                  // 任务优先级，数字越大优先级越高
        &keyTask            // 任务句柄；不需要管理任务就写 NULL
    );
	
	xTaskCreate(&cooking_Task,"cookingTask",128,NULL,5,&cookingTask);
	xTaskCreate(&dht_Task,"dhtTask",128,NULL,2,&dhtTask);
	
	heating_sem = xSemaphoreCreateBinary();          //创建信号量
	
	vTaskStartScheduler ();         //启动调度
	
	
	while(1)
	{	
		
	}
	
	
}


