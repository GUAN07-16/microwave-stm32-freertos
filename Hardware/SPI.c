#include "SPI.h"

//spi1 初始化
//cs PA4 
//D0 PA6  MISO
//D1 PA7  MOSI
//SCLK PA5
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;    //cs
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET); //系统上电 / GPIO 初始化完成后，CS 必须是高电平

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;//SCLK
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;//MISO,输入
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入浮空模式
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;//MOSI
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;                     //主机
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//双线全双工
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64; //64M
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStruct.SPI_CRCPolynomial=7;
	
	SPI_Init(SPI1,&SPI_InitStruct);

	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
	
	SPI_Cmd(SPI1, ENABLE);
}