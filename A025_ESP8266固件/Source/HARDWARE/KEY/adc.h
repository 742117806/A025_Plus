/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: adc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-10
@Description: ADC����
@Function List: 
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/

#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

void Adc_Init(void);  //��ʼ��adc
u16  Get_Adc(u8 ch);  //��ȡchͨ����ADCֵ
//u16 Get_Adc_Average(u8 ch,u8 times); 
 
#endif 
