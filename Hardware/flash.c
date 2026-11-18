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


// /*设置主栈 MSP*/
////uint32_t addr 自动存入r0    MSR将r0放入SP寄存器 ,BX r14 的意思就是“执行完毕"
//__asm void MSR_SP(uint32_t addr)
//{
//	MSR MSP,r0         // 1. 把 r0 寄存器的值写入主栈指针 (MSP)
//	BX  r14            // 2. 函数返回 (跳转回调用它的 C 语言代码)
//	
//}


////对内存地址解引用，只会取出内存里存的原始数值
////Flash 里向量表存储的本质是原始 32 位数据，优先用uint32_t*（数据指针）
//            /*跳转A区程序,A区起始地址传入*/
//void LOAD_A(uint32_t addr)
//{
//	
//	if( *(uint32_t *)addr>= 0x20000000 && *(uint32_t *)addr<= 0x20004FFF)   //在内存范围内
//	{
//		/*传入addr只是数字,(uint32_t *)addr强转为指向该地址addr的内存指针,再解引用处实际APP在flasj内的起始地址*/
//		
//		MSR_SP (*(uint32_t * ) addr);            //设置主栈指针,栈顶:addr
//	
//		// 第一步：读Flash，(uint32_t *)(addr+4)：把这个数字转成uint32_t*指针，再解引用,代表这个是内存地址拿到向量表里存的那个地址数字
//		//adr + 4复位函数入口,第一条代码的地址
//		
//      uint32_t entry_num = *(uint32_t *)(addr+4);
//		// 第二步：把这个数字，强制转成函数指针类型
//     load_A = (load_a) entry_num;
//		 load_A();                                 //():更新PC,跳转运行新的addr后的程序
//	}
//	else
//	{
//		u1_printf("\r\nOTA跳转A分区失败自动进入命令行\r\n");		
//	}
//	
//	
//}



 /*设置主栈MSP + 切回MSP + 跳A区,一口气做完永不返回
   sp=A区栈顶值  entry=A区复位向量值
   注意:全程只用寄存器r0/r1/r2,不碰栈——因为执行到一半栈就被换了,
   任何"读写内存/压栈"行为都会踩到刚换过来的陌生栈,所以一条都没有*/
__asm void MSR_SP(uint32_t sp, uint32_t entry)
{
	MSR MSP,r0         // 1. 原行保留:把r0(A区栈顶)写入MSP寄存器
	MRS r2,CONTROL     // 2. 新增:读出CONTROL寄存器到r2,它的bit1(SPSEL)=1表示当前用PSP
	BIC r2,r2,#2       // 3. 新增:BIC=按位清零,把bit1清0 -> 声明"从现在起用MSP"
	MSR CONTROL,r2     // 4. 新增:写回CONTROL,切换生效,告别PSP
	ISB                // 5. 新增:同步屏障,等切换彻底落地再执行下一步
	BX  r1             // 6. 原来是BX r14(=返回调用者);现在BX r1=跳向A区入口,永不返回
}




 /*跳转A区程序,A区起始地址传入
   RTOS版比裸机版必须多做两件事:
   A.跳转前掐死所有在途中断:SysTick每1ms一拍,跳转瞬间大概率有一拍已挂号排队,
     向量表还指着B区,让它执行=B区ISR去操作A区startup正在碾的堆=死机
   B.切回MSP:任务平时用PSP,不切的话MSR MSP等于白换,A区全程跑在B区遗留栈上*/
void LOAD_A(uint32_t addr)
{
	//先把两个数读出来存局部变量(跳转的活必须在"跳走之前"用C干完,跳走就回不来了)
	uint32_t sp = *(uint32_t *)addr;            //A区栈顶值(bin头4字节)
	uint32_t entry_num = *(uint32_t *)(addr+4); //A区复位向量(bin第4~7字节,你原来的第一步理解原样保留)

	if( sp >= 0x20000000 && sp <= 0x20004FFF )  //栈顶指纹校验,在内存范围内(拦垃圾固件,原逻辑)
	{
		/*==跳转前清理四连==*/
		__disable_irq();               //关总中断(PRIMASK=1):从这行起任何中断都不再响应
		SysTick->CTRL = 0;             //停RTOS心跳:清ENABLE,不再计数不再申请中断
		SysTick->VAL  = 0;             //计数值清零,顺手清掉COUNTFLAG
		TIM3->SR = 0;                  //清TIM3的pending源头(10ms按键那拍可能已置UIF)
		TIM4->SR = 0;                  //清TIM4的pending源头
		NVIC->ICPR[0] = 0xFFFFFFFF;    //清NVIC挂号处:把所有"已排队等执行"的在途中断全部撤销
		/*==补刀:只清SR不够,TIM3开机起永远在跑,10ms后UIF重新置位会再挂号;
			 A区enable_irq一开就取A区weak向量(B .死循环)=整机卡死在跳转后。
			 必须停计数器本体+关UIE,双关==*/
		TIM_Cmd(TIM3, DISABLE);                    //停TIM3计数器(10ms扫描那位)
		TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);//关它的中断申请线
		TIM_Cmd(TIM4, DISABLE);                    //TIM4顺手(平时已DISABLE,防加热中升级)
		TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
		
		/*传入addr只是数字,强转指针再解引用=取出向量表里存的原始数值(你原来的理解,原样保留)
		  原版"指针变量load_A();更新PC跳转"退役:因为新MSR_SP是换栈+跳转一口气,中间没有
		  落地回C的机会(一旦回C,代码就在用刚换掉的陌生栈,局部变量全悬空)*/
		MSR_SP (sp, entry_num);        //换栈+切MSP+跳A区入口,此行永不返回
	}
	else
	{
		u1_printf("\r\nOTA跳转A分区失败自动进入命令行\r\n");
	}
}






