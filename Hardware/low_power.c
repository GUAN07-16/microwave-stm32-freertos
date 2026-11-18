#include "low_power.h"

/**
 * @brief       进入低功耗前准备函数
 * @param       待运行任务运行时间
 * @retval      无
 */
void PRE_SLEEP_PROCESSING(u32 ulExpectedIdleTime)
{
  /* 关闭部分外设时钟，仅作演示 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,DISABLE);
}

/**
 * @brief      退出低功耗前准备函数
 * @param       待运行任务运行时间
 * @retval      无
 */
void POST_SLEEP_PROCESSING(u32 ulExpectedIdleTime)
{
  /* 重新打开部分外设时钟，仅作演示 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
}





