/*
*LCD.h头文件
*移植自网上开源代码
*消去寄存器别名宏定义，直接使用寄存器，适配STM32标准库
*/
#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"
#include "stdlib.h"
#include "spi.h"
#include "delay.h"


//LCD重要参数结构体
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				  //LCD ID
	u8  dir;			  //屏幕方向标记，0竖屏1横屏	
	u16	 wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令	 
}_lcd_dev; 	

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
/////////////////////////////////////用户配置区///////////////////////////////////	 
#define USE_HORIZONTAL  	 0 //设置屏幕横屏旋转 	0-0度旋转 1-90度旋转 2-180度旋转 3-270度旋转

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 128
#define LCD_H 160

//TFTLCD需要用到的IO口		   
extern u16  POINT_COLOR;//画笔颜色
extern u16  BACK_COLOR; //背景颜色

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define GPIO_TYPE  GPIOB  //GPIO分组
#define LED        6      //背光引脚        PB6
#define LCD_CS     9      //片选引脚        PB9
#define LCD_RS     7      //命令/数据选择引脚 PB7 
#define LCD_RST    8      //复位引脚        PB8


//QDtech全系列模块支持硬件SPI，速度很快，也可以用PWM调节背光
#define	LCD_LED PBout(LED) //LCD背光    		 PB6
//可以用软件方式驱动，速度慢，14帧左右；硬件SPI驱动最高可达28帧。

//GPIO置位操作（直接操作BSRR寄存器，移除BSHR别名宏）
#define	LCD_CS_SET  GPIO_TYPE->BSRR = 1 << LCD_CS    //片选拉高 	
#define	LCD_RS_SET  GPIO_TYPE->BSRR = 1 << LCD_RS    //命令/数据拉高  	  
#define	LCD_RST_SET GPIO_TYPE->BSRR = 1 << LCD_RST   //复位拉高			  

//GPIO清零操作（直接操作BRR寄存器，移除BCR别名宏）							    
#define	LCD_CS_CLR  GPIO_TYPE->BRR = 1 << LCD_CS     //片选拉低  	
#define	LCD_RS_CLR  GPIO_TYPE->BRR = 1 << LCD_RS     //命令/数据拉低  	 
#define	LCD_RST_CLR GPIO_TYPE->BRR = 1 << LCD_RST    //复位拉低			  

//颜色定义 RGB565
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //棕色
#define BRRED 			0XFC07 //棕红色
#define GRAY  			0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的背景色 
 
#define LIGHTGREEN     	0X841F //浅绿色
#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 		0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//画点
u16  LCD_ReadPoint(u16 x,u16 y); //读点
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);

u16 LCD_RD_DATA(void);//读取LCD数据									    
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);		   
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction );

//如果想提升刷屏速度，可以启用下面的宏，减少函数调用开销
//注意需要去lcd.c的void LCD_WR_DATA(u16 data)注释对应代码
/*
#if LCD_USE8BIT_MODEL==1//使用8位并行模式
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	DATAOUT(data<<8);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	}
	#else//使用16位并行模式
	#define LCD_WR_DATA(data){\
	LCD_RS_SET;\
	LCD_CS_CLR;\
	DATAOUT(data);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
	LCD_CS_SET;\
	} 	
#endif
*/
				  		 
#endif

