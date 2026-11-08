/*
*LCD.c文件
*移植自网上开源代码
*硬件：ST7735S 128*160 SPI TFT屏幕
*SPI：SPI1
*IO：PB6背光，PB7 RS，PB8 RST，PB9 CS
*/		
#include "lcd.h"

//全局结构体：保存屏幕核心参数（分辨率、ID、坐标指令、方向）
_lcd_dev lcddev;

//画笔颜色、背景颜色全局变量
u16 POINT_COLOR = 0x0000;   //画笔默认黑色
u16 BACK_COLOR  = 0xFFFF;   //背景默认白色
u16 DeviceCode;	 

/**
 * @brief   向LCD写入8位命令（寄存器指令）
 * @param   data：要发送的命令码
 * @note    RS拉低代表命令，CS拉低选中屏幕，SPI发送后释放片选
 */
void LCD_WR_REG(u8 data)
{ 
   LCD_CS_CLR;     //片选拉低，选中LCD
	 LCD_RS_CLR;	   //RS=0：表示传输【命令】
   SPI1_ReadWriteByte(data);
   LCD_CS_SET;	   //片选拉高，结束本次SPI传输
}

/**
 * @brief   向LCD写入8位显示数据
 * @param   data：要发送的数据
 * @note    RS拉高代表数据，用来写寄存器参数/GRAM像素数据
 */
void LCD_WR_DATA(u8 data)
{
   LCD_CS_CLR;     //片选拉低，选中LCD
	 LCD_RS_SET;	   //RS=1：表示传输【数据】
   SPI1_ReadWriteByte(data);
   LCD_CS_SET;     //释放片选
}

/**
 * @brief   写LCD寄存器（16bit寄存器值）
 * @param   LCD_Reg：寄存器地址（命令）
 * @param   LCD_RegValue：要写入寄存器的16位参数
 */
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);        //先发送寄存器地址命令
	LCD_WR_DATA(LCD_RegValue);	//再写入对应参数
}	   

/**
 * @brief   准备开始写GRAM显存（像素缓冲区）
 * @note    发送写GRAM指令，后续SPI数据都会当作像素颜色
 */
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/**
 * @brief   写入16bit像素颜色（RGB565）
 * @param   Data：RGB565 16位颜色值
 * @note    SPI是8位传输，先发高8bit，再发低8bit
 */
void Lcd_WriteData_16Bit(u16 Data)
{	
   LCD_CS_CLR;
   LCD_RS_SET;  
   SPI1_ReadWriteByte(Data>>8);  //发送颜色高字节
	 SPI1_ReadWriteByte(Data);     //发送颜色低字节
   LCD_CS_SET;
}

/**
 * @brief   在指定坐标绘制单个像素点
 * @param   x：X坐标
 * @param   y：Y坐标
 * @note    使用全局POINT_COLOR作为该点颜色
 */
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);	            //设置光标到目标坐标
	Lcd_WriteData_16Bit(POINT_COLOR); //写入像素颜色
}

/**
 * @brief   全屏填充单色，清屏函数
 * @param   Color：填充的RGB565颜色
 * @note    先设置全屏窗口，循环写入像素颜色
 */
void LCD_Clear(u16 Color)
{
  unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   //窗口设为整个屏幕
	LCD_CS_CLR;
	LCD_RS_SET;
	for(i=0;i<lcddev.height;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			Lcd_WriteData_16Bit(Color);
		}
	}
	LCD_CS_SET;
} 

/**
 * @brief   LCD控制引脚GPIO初始化（PB6/PB7/PB8/PB9）
 * @note    PB6:背光LED，PB7:RS命令/数据，PB8:RST复位，PB9:CS片选，全部推挽输出
 */
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);	//使能GPIOB外设时钟
	
	//PB6 PB7 PB8 PB9全部配置为推挽输出，速度50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief   LCD硬件复位
 * @note    拉低复位引脚，保持一段时间后拉高，等待屏幕上电稳定
 */
void LCD_RESET(void)
{
	LCD_RST_CLR;        //复位拉低
	Delay_ms(100);	
	LCD_RST_SET;        //复位拉高，释放复位
	Delay_ms(50);
}

/**
 * @brief   LCD屏幕初始化
 * @note   ST7735S寄存器初始化序列，配置帧率、电源、伽马、颜色模式，最后点亮背光并清屏
 */	 	 
void LCD_Init(void)
{  
	SPI1_Init(); //硬件SPI1初始化
	LCD_GPIOInit();//LCD GPIO初始化										 
 	LCD_RESET(); //LCD 复位
//************* ST7735S初始化寄存器序列**********//	
	LCD_WR_REG(0x11);//Sleep exit 退出休眠模式
	Delay_ms(120);	
	//ST7735R Frame Rate 帧率配置
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_REG(0xB2); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_REG(0xB3); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 	
	LCD_WR_REG(0xB4); //Column inversion 列反转
	LCD_WR_DATA(0x07); 	
	//ST7735R Power Sequence 电源配置
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0xC5); 
	LCD_WR_REG(0xC2); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC3); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0xEE); 
	LCD_WR_REG(0xC5); //VCOM 电压配置
	LCD_WR_DATA(0x0E); 	
	LCD_WR_REG(0x36); //MX, MY, RGB模式 屏幕扫描方向
	LCD_WR_DATA(0xC0); 
	//ST7735R Gamma Sequence 伽马校正
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1a); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x2f); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x20); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x1f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x23); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x00); 	
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x10); 
	LCD_WR_REG(0xe1); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x2c); 
	LCD_WR_DATA(0x29); 
	LCD_WR_DATA(0x2e); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x3f); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0x10);  	
	LCD_WR_REG(0x2a); //X坐标范围
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);
	LCD_WR_REG(0x2b); //Y坐标范围
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	LCD_WR_REG(0xF0); //使能测试命令  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xF6); //关闭RAM省电模式 
	LCD_WR_DATA(0x00); 	
	LCD_WR_REG(0x3A); //设置颜色深度 65K彩色 RGB565
	LCD_WR_DATA(0x05); 
	LCD_WR_REG(0x29);//开启显示	 	
  LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
	LCD_LED=1;//点亮背光	 
	LCD_Clear(WHITE);//清全屏白色
}
 
/**
 * @brief   设置屏幕绘图窗口（矩形区域）
 * @param   xStar:窗口起始X坐标
 * @param   yStar:窗口起始Y坐标
 * @param   xEnd:窗口结束X坐标
 * @param   yEnd:窗口结束Y坐标
 * @note    设置完成后，后续GRAM写入只会在这个矩形区域生效
 */ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xStar);		
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yStar);		
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

/**
 * @brief   设置光标到单个像素坐标
 * @param   Xpos：目标X坐标
 * @param   Ypos：目标Y坐标
 * @note    本质是把窗口设置成1个像素大小
 */ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/**
 * @brief   设置屏幕旋转方向
 * @param   direction：0=0°竖屏，1=90°横屏，2=180°，3=270°
 * @note    修改ST7735S的0x36寄存器，改变行列扫描顺序，同步更新lcddev分辨率
 */ 
void LCD_direction(u8 direction)
{ 
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(0<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(0<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(0<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(0<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}
