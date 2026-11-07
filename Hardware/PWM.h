#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h"

void TIM1_PWM_Init(void);
void TIM2_PWM_Init(void);
void Door_Close(void);   
void Door_Open(void);
void Heater_Set(uint16_t duty);
void Turntable_Start(void);
void Turntable_Stop(void);

//改变CCR的值,  50%占空比 -> 32000, 70%占空比->44800,100%占空比->64000
#define HEATER_LOW   32000   /* 50% */
#define HEATER_MED   44800   /* 70% */
#define HEATER_HIGH  64000   /* 100%,恒高 */

#endif
