#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

//********************************************************************************  

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�
__asm void INTX_DISABLE(void)
{
	CPSID I;		  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE I;		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


/*****************************************************************
 * ��������Get_ChipID
 * ����  ����ȡоƬID
 * ����  ����
 * ���  ����
 * ˵��  ��96λ��ID��stm32Ψһ��ݱ�ʶ��������8bit��16bit��32bit��ȡ
           �ṩ�˴�˺�С�����ֱ�ʾ����
****************************************************************/
u8 Get_ChipID(void)
{
    u32 ChipUniqueID[3];
    //��ַ��С����,�ȷŵ��ֽڣ��ٷŸ��ֽڣ�С��ģʽ
    //��ַ��С����,�ȷŸ��ֽڣ��ٷŵ��ֽڣ����ģʽ
 
	ChipUniqueID[2] = *(__IO u32*)(0X1FFFF7E8);  // ���ֽ�
	 
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	 
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // ���ֽ�
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

