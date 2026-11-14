#include "Timer.h"                  // Device header

void Timer_Init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	TIM_InternalClockConfig(TIM3);                       //给 TIM3 选择内部时钟源
/* 
	对于 TIM2/TIM3/TIM4（挂在 APB1） AHB(默认):72M
	APB1 2分频:原本APB1=AHB/2=36M ,但是输出:硬件自动2*APB1 =72M
	APB1 1分频:APB1= AHB=72M
 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //APB1->72M
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //计数器向上读
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;                  //ARR:自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                  //PSC分频
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);                //72M,计算得一次更新中断触发时间为10ms
	
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;           //定时器抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                         //使能更新中断
	TIM_Cmd(TIM3, ENABLE);
	
	
}
void TIM4_Init(void)                    //1s进入一次中断用于加热时间--
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	TIM_InternalClockConfig(TIM4);                       //给 TIM4 选择内部时钟源
/* 
	对于 TIM2/TIM3/TIM4（挂在 APB1） AHB(默认):72M
	APB1 2分频:原本APB1=AHB/2=36M ,但是输出:硬件自动2*APB1 =72M
	APB1 1分频:APB1= AHB=72M
 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        //APB1->72M
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //计数器向上读
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;                  //ARR:自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;                  //PSC分频
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);                //72M,计算得一次更新中断触发时间为10ms
	
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;           //定时器抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);                         //使能更新中断
	TIM_Cmd(TIM4, DISABLE);
	
	
}


