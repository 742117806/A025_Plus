/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: presetTimer.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM数据 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _H_PWMDATA_
#define _H_PWMDATA_

#include "sys.h"
#include "timer.h"

#define PRGM_STEP     24 //定时时段


//PWM存储结构
//时钟 分钟 PWM1 pwm2  PWM3 PWM4
typedef struct _timer
{
	u8 enable;  //开关
	u8 hour;  //小时
	u8 minute; //分钟
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc;	//nc 没有使用，只是为了保证字节对齐
}TIMER;

//手动设置定时
typedef struct _SunSet{
	u8 s_hour;
	u8 s_minute;
	u8 e_hour;
	u8 e_minute;
	STRUCT_PWM pwmDay; //白天亮度
	STRUCT_PWM pwmMoon;//月光亮度
}STRUCT_MT;

extern TIMER TimerProg[PRGM_STEP];//实际调光
extern const TIMER presetTimer[PRGM_STEP];
extern STRUCT_MT esayTimer;
extern const STRUCT_MT presetEsayTimer; //预置 easy timer

#endif
 
