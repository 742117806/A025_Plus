/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: presetTimer.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM���� 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _H_PWMDATA_
#define _H_PWMDATA_

#include "sys.h"
#include "timer.h"

#define PRGM_STEP     24 //��ʱʱ��


//PWM�洢�ṹ
//ʱ�� ���� PWM1 pwm2  PWM3 PWM4
typedef struct _timer
{
	u8 enable;  //����
	u8 hour;  //Сʱ
	u8 minute; //����
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc;	//nc û��ʹ�ã�ֻ��Ϊ�˱�֤�ֽڶ���
}TIMER;

//�ֶ����ö�ʱ
typedef struct _SunSet{
	u8 s_hour;
	u8 s_minute;
	u8 e_hour;
	u8 e_minute;
	STRUCT_PWM pwmDay; //��������
	STRUCT_PWM pwmMoon;//�¹�����
}STRUCT_MT;

extern TIMER TimerProg[PRGM_STEP];//ʵ�ʵ���
extern const TIMER presetTimer[PRGM_STEP];
extern STRUCT_MT esayTimer;
extern const STRUCT_MT presetEsayTimer; //Ԥ�� easy timer

#endif
 
