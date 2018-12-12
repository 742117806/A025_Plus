/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM�������
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "my_type.h"

#define MAX_PWM		101	//���PWMֵ [������100ʱ������Ϊ99]
#define PWM_SPEED	20000  //20K
#define SYS_CLK		72000000

//#define PWM_SPEED	(SYS_CLK/MAX_PWM/PWM_SPEED)   //20k

#define PWM_NUM		6  //PWM��·��

typedef struct _pwm{
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
}STRUCT_PWM; //PWM�ṹ��

u8 UpdatePWM(void); //����PWM,Ҫ����ѭ����ִ��
void SetPwm(u8 pwm1,u8 pwm2, u8 pwm3, u8 pwm4, u8 pwm5,u8 pwm6); //���ý���PWM
void SetPwmA(u8 p1,u8 p2, u8 p3, u8 p4, u8 p5,u8 p6); //ֱ����������
void PWM_Init(void);  //��ʼ��PWM
u8	GetPwm(u8 ch); //��ȡָ��ͨ��������ֵ ch = 1~6 
u8 GetPwmUpataState(void);//��ȡPWM��ǰ�Ƿ��б仯��0:�ޱ仯,!0�б仯

STRUCT_PWM *GetPwmB(void);
void SetPwmB(STRUCT_PWM *pPwm,u8 mode);

#endif
