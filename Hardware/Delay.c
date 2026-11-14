#include "Delay.h"

///**
//  * @brief  微秒级延时
//  * @param  xus 延时时长，范围：0~233015
//  * @retval 无
//  */
//void Delay_us(uint32_t xus)
//{
//	SysTick->LOAD = 72 * xus;				//设置定时器重装值
//	SysTick->VAL = 0x00;					//清空当前计数值
//	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
//	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
//	SysTick->CTRL = 0x00000004;				//关闭定时器
//}

///**
//  * @brief  毫秒级延时
//  * @param  xms 延时时长，范围：0~4294967295
//  * @retval 无
//  */
//void Delay_ms(uint32_t xms)
//{
//	while(xms--)
//	{
//		Delay_us(1000);
//	}
//}
// 
///**
//  * @brief  秒级延时
//  * @param  xs 延时时长，范围：0~4294967295
//  * @retval 无
//  */
//void Delay_s(uint32_t xs)
//{
//	while(xs--)
//	{
//		Delay_ms(1000);
//	}
//} 



#define DEMCR    (*(volatile unsigned *)0xE000EDFC)  /* 调试寄存器 */
#define DWT_CTRL (*(volatile unsigned *)0xE0001000)  /* DWT 控制寄存器 */
#define CYCCNT   (*(volatile unsigned *)0xE0001004)  /* 周期计数器 */

void Delay_Init(void)
{
    DEMCR |= (1 << 24);        /* TRCENA:开调试单元 */
    CYCCNT = 0;
    DWT_CTRL |= 1;             /* CYCCNTENA:计数器开始跑 */
}

void Delay_us(uint32_t xus)
{
    uint32_t start = CYCCNT;
    uint32_t ticks = 72 * xus;
    while((CYCCNT - start) < ticks);
}

void Delay_ms(uint32_t xms)
{
    while(xms--) Delay_us(1000);
}



