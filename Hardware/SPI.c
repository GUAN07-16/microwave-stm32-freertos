#include "SPI.h"

// 硬件 SPI 外设的 SCK/MISO/MOSI 必须使用芯片规定的复用引脚，不能随便挑任意 IO,只有 CS 片选是普通 GPIO，随便分配
//CS       PB9 
//D0       PA6  MISO
//D1       PA7  MOSI
//SCLK     PA5
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;    //cs
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_SET);       //系统上电 / GPIO 初始化完成后，CS 必须是高电平

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;   //SCLK
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;   //MISO,输入
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入浮空模式
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;   //MOSI
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

  //SPI初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;                     //主机
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//双线全双工
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;                   //SPI_CPOL_Low + SPI_CPHA_1Edge  = Mode 0
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;                 //SPI_CPHA_1Edge → CPHA=0       
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64;  //   72M/64 = 1.125MHz
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStruct.SPI_CRCPolynomial=7;
	
	SPI_Init(SPI1,&SPI_InitStruct);

	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
	
	SPI_Cmd(SPI1, ENABLE);
}



                             //发送伪数据0xFF
uint8_t SPI1_ReadWriteByte(uint8_t txd)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, txd);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

 
void SPI1_Write(uint8_t *wdata, uint16_t datalen)
{
    uint16_t i;
    for(i = 0; i < datalen; i++)
    {
        SPI1_ReadWriteByte(wdata[i]);
    }
}

                     //读取后存储地址
void SPI1_Read(uint8_t *rdata, uint16_t datalen)
{
    uint16_t i;
    for(i = 0; i < datalen; i++)
    {
        rdata[i] = SPI1_ReadWriteByte(0xFF);
    }
}





