/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: fan.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-10
@Description: ���ȵ��ٹ���
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/

#ifndef __TIMER_FAN_H
#define __TIMER_FAN_H
#include "sys.h"

void FAN_init(void);	//���ȳ�ʼ��
void SetFanSpeed(u8 speed);  //���÷���ת��
u8	GetFanSpeed(void); //��ȡ����ת��

#endif
