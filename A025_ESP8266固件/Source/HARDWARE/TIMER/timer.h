/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM输出操作
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "my_type.h"

#define MAX_PWM		101	//最大PWM值 [亮度是100时这里设为99]
#define PWM_SPEED	20000  //20K
#define SYS_CLK		72000000

//#define PWM_SPEED	(SYS_CLK/MAX_PWM/PWM_SPEED)   //20k

#define PWM_NUM		6  //PWM的路数

typedef struct _pwm{
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
}STRUCT_PWM; //PWM结构体

u8 UpdatePWM(void); //更新PWM,要放在循环中执行
void SetPwm(u8 pwm1,u8 pwm2, u8 pwm3, u8 pwm4, u8 pwm5,u8 pwm6); //设置渐变PWM
void SetPwmA(u8 p1,u8 p2, u8 p3, u8 p4, u8 p5,u8 p6); //直接设置亮度
void PWM_Init(void);  //初始化PWM
u8	GetPwm(u8 ch); //获取指定通道的亮度值 ch = 1~6 
u8 GetPwmUpataState(void);//获取PWM当前是否有变化，0:无变化,!0有变化

STRUCT_PWM *GetPwmB(void);
void SetPwmB(STRUCT_PWM *pPwm,u8 mode);

#endif
