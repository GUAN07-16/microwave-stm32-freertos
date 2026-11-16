/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "key.h"
#include "PWM.h"
#include "FreeRTOS.h"
#include "semphr.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
//void SysTick_Handler(void)
//{
//}


extern KEY_CB KEY1;
extern KEY_CB KEY2;
extern void Key_Process(KEY_CB * KEY);
extern volatile uint8_t time;    //extern不需要赋值
extern SemaphoreHandle_t heating_sem; 
extern SemaphoreHandle_t iap_sem;
void TIM3_IRQHandler(void)             
{
  if (TIM_GetITStatus(TIM3,TIM_IT_Update) == SET) //轮询 TIM_GetFlagStatus,检测是否产生中断
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);         //清除中断标志位
	
		Key_Process(&KEY1);
		Key_Process(&KEY2);

	}
	
}

void TIM4_IRQHandler(void)             
{
	
	// 定义局部变量，初始是pdFALSE
	BaseType_t xWoken = pdFALSE;
	
  if (TIM_GetITStatus(TIM4,TIM_IT_Update) == SET) //轮询 TIM_GetFlagStatus,检测是否产生中断
	{
		   TIM_ClearFlag(TIM4, TIM_FLAG_Update);         //清除中断标志位
	  
				xSemaphoreGiveFromISR(heating_sem, &xWoken);        //释放信号量
				
			 //如果本次释放信号量唤醒了优先级更高的任务，这行代码就会在中断结束之后，立刻切去跑这个高优先级任务；没唤醒就什么都不做
				portYIELD_FROM_ISR (xWoken);
		
	}
	
}

void DMA1_Channel5_IRQHandler(void)          //DMA一次搬运完成则置位TC
{
	// 定义局部变量，初始是pdFALSE
	BaseType_t xWoken = pdFALSE;
  if(DMA_GetITStatus(DMA1_IT_TC5)==SET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC5);
		
		xSemaphoreGiveFromISR(iap_sem, &xWoken);        //释放信号量
	 //如果本次释放信号量唤醒了优先级更高的任务，这行代码就会在中断结束之后，立刻切去跑这个高优先级任务；没唤醒就什么都不做
		portYIELD_FROM_ISR (xWoken);
		
	}
}





/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
