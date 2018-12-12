/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : ºý¶Á³æ
@Version : 1.0
@Date    : 2015-8-12
@Description: ¶¨Ê±¼ÆËã
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _TIMER_FUNCTION_H	
#define _TIMER_FUNCTION_H
#include "sys.h"
#include "pwmData.h"
#include "rtc.h"

#define		HOUR_RADIX	60	 //60Ãë
extern _calendar_obj now;
//////////////////
u8 findCurrectPwm(u8 hour,u8 minute);
void TimerCopy(TIMER* target,TIMER* source);
void SortTimer(void);
void SetPwmByTimer(u8 hour,u8 minute);
u8 GetMaxTimerStep(void);

#endif
