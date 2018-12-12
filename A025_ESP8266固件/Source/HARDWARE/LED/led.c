/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: led.c
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: LED指示灯驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "led.h"

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE );
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);        //JTAG-DP 失能 + SW-DP使能 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
// GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PB.5 输出高
 GPIO_ResetBits(GPIOA, GPIO_Pin_15);						 //PB.5 输出低

//------------wifi led
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_3);						 //PB.3 输出高


}
/*------------end of file----------*/

