/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: fan.c
@Author  : ������
@Version : 1.0
@Date    : 2015-8-10
@Description: ���ȵ��ٹ���
@Function List: 
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/
#include "fan.h"
#include "stm32f10x_tim.h"

u8 fanspeed;  //����ת��

#define  SET_FAN_SPEED(X)	TIM_SetCompare2(TIM2,X);

#define FAN_MAX_PWM		100		//���PWMֵ
#define FAN_PWM_SPEED	20000		//20K
#define FAN_SYS_CLK		72000000

//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

	//���ø�����Ϊ�����������,���TIM4 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO

	//��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM4 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
//	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC2
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 
//	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 
//	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 

//	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
//	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM4
}

//���ȳ�ʼ��
void FAN_init(void)
{
	TIM2_PWM_Init(FAN_MAX_PWM,(FAN_SYS_CLK/FAN_MAX_PWM/FAN_PWM_SPEED));
	SetFanSpeed(0);
}

//���÷���ת��
void SetFanSpeed(u8 speed)
{
	if (speed > 100)speed = 100; //���ֻ��100

	fanspeed = speed;
	SET_FAN_SPEED(fanspeed);
}

//��ȡ����ת��
u8	GetFanSpeed(void)
{
	return fanspeed;
}


