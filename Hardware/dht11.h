#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
}DHT_CB;

void DHT_CB_Start(DHT_CB *io);
void DHT_CB_Read(DHT_CB *io);
u8 DHT_Read_Byte(DHT_CB *io);
u8 DHT_Read_Data(u8 *temp,u8 *humi,GPIO_TypeDef * port,u16 pin,DHT_CB *io);
void chushi(DHT_CB *io);
u8 readpin(DHT_CB *io);

#endif

















