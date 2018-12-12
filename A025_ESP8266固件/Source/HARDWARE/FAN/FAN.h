/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: fan.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-10
@Description: 风扇调速功能
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/

#ifndef __TIMER_FAN_H
#define __TIMER_FAN_H
#include "sys.h"

void FAN_init(void);	//风扇初始化
void SetFanSpeed(u8 speed);  //设置风扇转速
u8	GetFanSpeed(void); //获取风扇转速

#endif
