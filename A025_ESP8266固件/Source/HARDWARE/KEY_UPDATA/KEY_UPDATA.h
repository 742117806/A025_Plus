/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: led.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: LED指示灯驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __KEY_UPDATA_H
#define __KEY_UPDATA_H	 
#include "sys.h"
 
#define BEEP_PIN		PCout(13)// 蜂鸣器
#define Gpio_Beep_pin		GPIO_Pin_13
void Beep_Init(void);//初始化

//#define KEY_Updata	 PBin(2)	//PB2  WK_UP
#define KEY_Updata  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)//读取按键


void KEY_Updata_Init(void);//IO初始化

#endif
