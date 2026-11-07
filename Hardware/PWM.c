#include "PWM.h"                  // Device header

//180   50Hz->20ms    ARR:20000   占空比 = CRR(500-2500)/ARR+1     2.5%(0.5ms,CCR=500) - 12.5%(2.5ms,CCR=2500)->旋转 (180)
//360   CCR = 1500->不转  >或<1500往某一方向一直转
//TIM2 PWM初始化控制180°舵机开关门及加热片功率


void TIM2_PWM_Init(void)                  
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          //TIM2挂在APB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);         //PWM输出IO口
	
	//TIM2_CH1 -> PA0
	//TIM2_CH2 -> PA1
	//TIM2_CH3 -> PA2
  //TIM2_CH4 -> PA3
	
	/* 
	对于 TIM2/TIM3/TIM4（挂在 APB1） AHB(默认):72M
	APB1 2分频:原本APB1=AHB/2=36M ,但是输出:硬件自动2*APB1 =72M
	APB1 1分频:APB1= AHB=72M
 */
	
	
	//设置该引脚为复用输出功能,输出 TIM2 CH2 的 PWM 脉冲波形
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //复用推挽输出:驱动强,速度快
	//CH2 = PA1 门舵机:开门 CCR=2500(2.5s)、关门 CCR=500(0.5ms).CH3 = PA2接继电器控制加热片.CH0 = PA0,360舵机
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	TIM_InternalClockConfig(TIM2);                        //使用内部 APB1 总线给的 72M 时钟
	//初始化 TIM2
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000-1;		    //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;	    	//PSC 只是分频，它改的是整个 PWM 的周期，不决定高低电平的比例。     
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                      //TIM2 是普通定时器，没有重复计数器
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);        //初始化 TIM2
	
	
	//初始化 TIM2 Channel2,Channel3,Channel1 PWM 模式
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择 PWM 模式1->舵机只识别每 20ms 开头的一段高电平脉冲
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性高
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);                      //初始化外设 TIM2 OC1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);                      //初始化外设 TIM2 OC2
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);                      //初始化外设 TIM2 OC3
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);             //使能捕获比较寄存器 CCR 的预装载(CCR 的影子寄存器)
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
	TIM_Cmd(TIM2, ENABLE);
	
	
}


//0.05hzPWM信号用于继电器

void TIM1_PWM_Init(void)               
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);          //TIM1挂在APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);         //PWM输出IO口
	
	//TIM1_CH1 -> PA8
	//TIM1_CH2 -> PA9
	//TIM1_CH3 -> PA10
	//TIM1_CH4 -> PA11

	/*
	  TIM1挂载 APB2
	  AHB:72M，APB2默认1分频，APB2总线=72M
	  APB2定时器时钟直接等于APB2总线时钟，没有自动×2倍频
	  TIM1内核时钟 =72MHz，和TIM2/TIM3/TIM4时钟频率相同
	*/

	//设置该引脚为复用输出功能,输出 TIM1 CH1 的 PWM 脉冲波形
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //复用推挽输出:驱动强,速度快
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	TIM_InternalClockConfig(TIM1);                        //使用内部 APB2 总线给的 72M 时钟
	//初始化 TIM1
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period =   64000 - 1;    //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler =22500 - 1;   	  //PSC 只是分频，它改的是整个 PWM 的周期，不决定高低电平的比例。     
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;    //TIM1是高级定时器，有重复计数器
	
	/*
	 RCR 作用：**不是每次溢出都触发更新事件，要溢出 RCR+1 次之后，才产生 1 次更新事件
   RCR=0：每溢出 1 次，直接产生更新事件 → 行为和普通 TIM2/3/4 完全一模一样
	 RCR=2：溢出 3 次，才触发 1 次更新事件；中间 2 次溢出不会产生更新事件、不会进更新中断
  */
	
	
	//初始化 TIM1 Channel1 PWM 模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);        //初始化 TIM1
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择 PWM 模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性高
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);                      //初始化外设 TIM1 OC1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //高级定时器主输出使能,打开 PWM 总闸门，否则没有波形输出

	//关闭CCR预装载，修改CCR立即生效,随时改变加热状态不需要等下一个周期
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);    //若使能CCR 的值存在影子寄存器，要等一个周期完成到来才把新值送入硬件比较寄存器生效。
	
  TIM_Cmd(TIM1, ENABLE);
	

}

/*PA8加热*/
void Heater_Set(uint16_t duty)
{
	 TIM_SetCompare1(TIM1, duty); 
}



/*PA1控制180舵机*/
void Door_Close(void)     
{ 
 TIM_SetCompare2(TIM2,2500 ); 
}
 void Door_Open(void)     
{ 
 TIM_SetCompare2(TIM2, 500); 
}


//PA0控制360舵机
void Turntable_Stop(void)
{ 
	TIM_SetCompare1(TIM2, 1500);
	
}
 void Turntable_Start(void)
{ 
	TIM_SetCompare1(TIM2, 1200);
}










