/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: led.c
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: LEDָʾ������ 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "led.h"

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE );
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);        //JTAG-DP ʧ�� + SW-DPʹ�� 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
// GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PB.5 �����
 GPIO_ResetBits(GPIOA, GPIO_Pin_15);						 //PB.5 �����

//------------wifi led
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_3);						 //PB.3 �����


}
/*------------end of file----------*/

