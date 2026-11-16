#include "flash.h"  
//片内flash

load_a load_A;  //声明一个函数指针变量 Load_A

// 从第 start 页开始擦除 num 个页 (STM32F103C8T6 每页 1024 字节)
void STMF103_32_Erase(uint16_t start, uint16_t num) 
{
    uint16_t i;
    
    // 防错校验：C8T6 最多 64 页 (0~63)，超出范围直接拦截，防止越界
    if (start + num > 64) return;
    
    FLASH_Unlock(); // 解锁片内 flash
    
    // 清除可能残留的状态标志位，保证擦除指令能被正常执行
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    

    for(i = 0; i < num; i++)
    { 
        
        FLASH_ErasePage(0x08000000 + (start + i) * 1024);    //f103片内flash每页1024字节
    }
    
    FLASH_Lock();  
}
                                   //一次写入一个字4字节,32位  字节个数
void STMF103_32_Write(uint32_t Start_addr, uint32_t *wdata, uint32_t wnum)
{
    
    if (wdata == 0 || wnum == 0) return;          // 空数据拦截
    if ((Start_addr & 0x03) != 0) return;         // 地址没有 4 字节对齐，拦截
    
    FLASH_Unlock(); // 解锁片内 flash
	
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    //即使传入非4倍数的 wnum，也能退出
    while (wnum >= 4)
    {
        FLASH_ProgramWord(Start_addr, *wdata);
        wnum-=4;             
        Start_addr+= 4;
        wdata++;           //32位的指针变量自增后自动指向下一个指针变量
    }
    FLASH_Lock(); 
}


 /*设置主栈 MSP*/
//uint32_t addr 自动存入r0    MSR将r0放入SP寄存器 ,BX r14 的意思就是“执行完毕"
__asm void MSR_SP(uint32_t addr)
{
	MSR MSP,r0         // 1. 把 r0 寄存器的值写入主栈指针 (MSP)
	BX  r14            // 2. 函数返回 (跳转回调用它的 C 语言代码)
	
}


//对内存地址解引用，只会取出内存里存的原始数值
//Flash 里向量表存储的本质是原始 32 位数据，优先用uint32_t*（数据指针）
            /*跳转A区程序,A区起始地址传入*/
void LOAD_A(uint32_t addr)
{
	
	if( *(uint32_t *)addr>= 0x20000000 && *(uint32_t *)addr<= 0x20004FFF)   //在内存范围内
	{
		/*传入addr只是数字,(uint32_t *)addr强转为指向该地址addr的内存指针,再解引用处实际APP在flasj内的起始地址*/
		
		MSR_SP (*(uint32_t * ) addr);            //设置主栈指针,栈顶:addr
	
		// 第一步：读Flash，(uint32_t *)(addr+4)：把这个数字转成uint32_t*指针，再解引用,代表这个是内存地址拿到向量表里存的那个地址数字
		//adr + 4复位函数入口,第一条代码的地址
		
      uint32_t entry_num = *(uint32_t *)(addr+4);
		// 第二步：把这个数字，强制转成函数指针类型
     load_A = (load_a) entry_num;
		 load_A();                                 //():更新PC,跳转运行新的addr后的程序
	}
	else
	{
		u1_printf("\r\nOTA跳转A分区失败自动进入命令行\r\n");		
	}
	
	
}




