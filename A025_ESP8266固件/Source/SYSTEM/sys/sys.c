#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

//********************************************************************************  

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断
__asm void INTX_DISABLE(void)
{
	CPSID I;		  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE I;		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


/*****************************************************************
 * 函数名：Get_ChipID
 * 描述  ：获取芯片ID
 * 输入  ：无
 * 输出  ：无
 * 说明  ：96位的ID是stm32唯一身份标识，可以以8bit、16bit、32bit读取
           提供了大端和小端两种表示方法
****************************************************************/
u8 Get_ChipID(void)
{
    u32 ChipUniqueID[3];
    //地址从小到大,先放低字节，再放高字节：小端模式
    //地址从小到大,先放高字节，再放低字节：大端模式
 
	ChipUniqueID[2] = *(__IO u32*)(0X1FFFF7E8);  // 低字节
	 
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	 
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
//0> ChipUniqueID[0] = 67246042
// 0> ChipUniqueID[1] = 57528970
// 0> ChipUniqueID[2] = 066DFF51
	//if((ChipUniqueID[0]!=0x67242559)||(ChipUniqueID[1]!=0x57528970)||(ChipUniqueID[2]!=0x0671FF51))//Wifi MAC 05D9
	//if((ChipUniqueID[0]!=0x67246042)||(ChipUniqueID[1]!=0x57528970)||(ChipUniqueID[2]!=0x066DFF51))//Wifi MAC 06B6
	if((ChipUniqueID[0]!=0x57094711)||(ChipUniqueID[1]!=0x30325434)||(ChipUniqueID[2]!=0x0671FF31))//Wifi MAC FD36
	{
//		SEGGER_RTT_printf(0,"ChipUniqueID[0] = %08x\r\n",ChipUniqueID[0]);
//		SEGGER_RTT_printf(0,"ChipUniqueID[1] = %08x\r\n",ChipUniqueID[1]);
//		SEGGER_RTT_printf(0,"ChipUniqueID[2] = %08x\r\n",ChipUniqueID[2]);

		return 0;
	}
	return 1;
}

