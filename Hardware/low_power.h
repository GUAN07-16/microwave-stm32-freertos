#ifndef __lower_power_H
#define __lower_power_H

#include "stm32f10x.h"
#include "FreeRTOSConfig.h"   /*µÍ¹¦ºÄµÄºêconfigUSE_TICKLESS_IDLE*/
void PRE_SLEEP_PROCESSING(u32 ulExpectedIdleTime);
void POST_SLEEP_PROCESSING(u32 ulExpectedIdleTime);



#endif


