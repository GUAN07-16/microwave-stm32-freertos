#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"  

#define KEY1_SHORT_FLAG  0x00000001 //状态标志位,按键短按

#define KEY1_LONG_FLAG   0x00000002 //状态标志位,按键长按

#define KEY2_SHORT_FLAG  0x00000004

#define KEY2_LONG_FLAG   0x00000008


extern volatile uint32_t KEYStaFlag;





#endif  




