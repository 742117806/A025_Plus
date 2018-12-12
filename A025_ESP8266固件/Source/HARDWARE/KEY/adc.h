/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: adc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-10
@Description: ADC功能
@Function List: 
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/

#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

void Adc_Init(void);  //初始化adc
u16  Get_Adc(u8 ch);  //获取ch通道的ADC值
//u16 Get_Adc_Average(u8 ch,u8 times); 
 
#endif 
