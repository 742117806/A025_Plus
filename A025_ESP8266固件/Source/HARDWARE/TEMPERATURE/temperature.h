/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ��ȡ�¶�
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H	
#include "sys.h"

typedef struct _temp{
	short temp;
	u16 adcVal;
}STRUCT_TEMP; //�¶Ƚṹ��

short GetTemperature(void); //��ȡ�¶�

#endif 
