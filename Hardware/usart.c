#include "usart.h"


uint8_t U1_RxBuff[4096];  //指定地址 0x20004000
uint8_t U1_TxBuff[1024];

//串口接收,DMA定长一次搬运完成后DMA_TC标志位置位,it.c释放信号量
//CNT=0 → 硬件置 TC 标志 → 触发 DMA 中断

void Usart1_DMA_Init(uint32_t bandrate,uint16_t Num)
{
    //USART1挂载在 APB2 总线,APB2 最高 72MHz
    //DMA1挂在AHB总线
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    /*串口初始化 PA9 ,PA10*/
  /* PA9  - USART1_TX Mode 复用推挽输出 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure); /*调用GPIO_InitStructure*/
    

    /*PA10  - USART1_TX  Mode: 浮空输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&GPIO_InitStructure); /*调用GPIO_InitStructure*/
    
    
    /*串口初始化  USART1 参数配置（8N1、无流控、收发都开） */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bandrate;
    USART_InitStructure.USART_HardwareFlowControl    = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode                       = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_Parity                       = USART_Parity_No;           //不校验
		USART_InitStructure.USART_StopBits                     = USART_StopBits_1;          //停止位
		USART_InitStructure.USART_WordLength                 = USART_WordLength_8b;       //一次传输8个数据位,开校验的话是9位
		USART_Init(USART1, &USART_InitStructure); /*调用USART_InitStructure*/

    USART_Cmd(USART1, ENABLE);
    
    //*DMA初始化*/
    /*USART1_RX（串口 1 接收）：固定连接在 DMA1 通道 5
      USART1_TX（串口 1 发送）：固定连接在 DMA1 通道 4    */
    
   DMA_InitTypeDef DMA_InitStructure;

  /* 复位初始化 DMA1 通道 5，确保寄存器干净 */
    DMA_DeInit(DMA1_Channel5);
  /*  源数据地址：外设（串口1数据寄存器） */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
  /*  目标地址：存储在 SRAM 里面的接收缓冲区首地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)U1_RxBuff;                     //接收内存数组
  /* 传输方向：外设作为数据源 (P-M) */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  /*  数据数目：DMA接收大小 */
    DMA_InitStructure.DMA_BufferSize = Num;    
  /*  外设地址不递增 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* 使能内存地址自动递增 */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /*  源数据字节大小 (8位) */     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*  目标数据字节大小 (8位) */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* 正常模式 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   /*  DMA 通道优先级为高级 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    /*  关闭内存到内存模式 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
		
		
    /* 完成 DMA 通道参数配置 */
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    /* 使能 DMA1 通道 5，开始接收数据 */
    DMA_Cmd(DMA1_Channel5, ENABLE);
    /* 使能串口1的 DMA 接收请求*/
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    /* 清除 DMA1 通道 5 的传输完成标志位 */
    DMA_ClearFlag(DMA1_FLAG_TC5);
    // 开启DMA通道5传输完成中断
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
		
    /*DMA中断配置 */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;   /* ≥3,合法(它调GiveFromISR) */
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
				
}


//...:个数和类型不确定
//通过串口1向电脑发送数据
void u1_printf(char *format, ...)
{
    uint16_t i;
    va_list listdata;

    va_start(listdata, format);
    vsprintf((char *)U1_TxBuff, format, listdata);
    va_end(listdata);

    for (i = 0; i < strlen((const char *)U1_TxBuff); i++)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, U1_TxBuff[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}



