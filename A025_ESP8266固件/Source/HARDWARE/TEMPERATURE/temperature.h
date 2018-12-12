/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 读取温度
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
}STRUCT_TEMP; //温度结构体

short GetTemperature(void); //读取温度

#endif 
