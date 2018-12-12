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
#include "timer.h"
#include "stm32f10x_tim.h"

//V1.1 20140904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
#define  SET_PWM1(X)	TIM_SetCompare1(TIM4,X)
#define  SET_PWM2(X)	TIM_SetCompare2(TIM4,X)
#define  SET_PWM3(X)	TIM_SetCompare3(TIM4,X)
#define  SET_PWM4(X)	TIM_SetCompare4(TIM4,X)
#define  SET_PWM5(X)	TIM_SetCompare1(TIM3,X)
#define  SET_PWM6(X)	TIM_SetCompare2(TIM3,X)

void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);

#define MIN_VALUE	2 //PWM��Сֵ

u16 tpwm[PWM_NUM]= {0,0,0,0,0,0};
u16 pwm[PWM_NUM]= {0,0,0,0,0,0};

u8 bIsUpdate = FALSE;  //�Ƿ����ڸ���PWM

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);        //TIM3ӳ�䵽PB�ڣ�������ӳ�� 

   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOC.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}


//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM4, ENABLE); //Timer3������ӳ��  TIM4_CH2->PB5    

	//���ø�����Ϊ�����������,���TIM4 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

	//��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM4 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
}

//��ʼ��6·PWM
void PWM_Init(void)
{
	TIM3_PWM_Init(MAX_PWM,(SYS_CLK/MAX_PWM/PWM_SPEED));//3	 //PSCֵ=ϵͳƵ��/PWM�׼�/PWMƵ��
	TIM4_PWM_Init(MAX_PWM,(SYS_CLK/MAX_PWM/PWM_SPEED)); //3

	//PWMֵ��Ϊ0
	SET_PWM1(0);
	SET_PWM2(0);
	SET_PWM3(0);
	SET_PWM4(0);
	SET_PWM5(0);
	SET_PWM6(0);
}

//ֱ����������
void SetPwmA(u8 p1,u8 p2, u8 p3, u8 p4, u8 p5,u8 p6)
{
	/*
	SET_PWM1(p1);
	SET_PWM2(p2);
	SET_PWM3(p3);
	SET_PWM4(p4);
	SET_PWM5(p5);
	SET_PWM6(p6);
	*/
	//PWMֵ��С��2����ֹ����
	if (p1)SET_PWM1(p1+MIN_VALUE);
	else SET_PWM1(0);

	if (p2)SET_PWM2(p2+MIN_VALUE); 
	else SET_PWM2(0);

	if (p3)SET_PWM3(p3+MIN_VALUE); 
	else SET_PWM3(0);

	if (p4)SET_PWM4(p4+MIN_VALUE); 
	else SET_PWM4(0);

	if (p5)SET_PWM5(p5+MIN_VALUE); 
	else SET_PWM5(0);

	if (p6)SET_PWM6(p6+MIN_VALUE); 
	else SET_PWM6(0);

//////////////////////////////////
	//��֤��ȡ������PWMֵһ��
	SetPwm(p1,p2,p3,p4,p5,p6);
	/*
	if (p1)pwm[0] = p1+MIN_VALUE;  //PWMֵ��С��2����ֹ����
	else pwm[0] = 0;

	if (p2)pwm[1] = p2+MIN_VALUE;
	else pwm[1] = 0;

	if (p3)pwm[2] = p3+MIN_VALUE;
	else pwm[2] = 0;

	if (p4)pwm[3] = p4+MIN_VALUE;
	else pwm[3] = 0;

	if (p5)pwm[4] = p5+MIN_VALUE;
	else pwm[4] = 0;

	if (p6)pwm[5] = p6+MIN_VALUE;
	else pwm[5] = 0;
	*/
	tpwm[0] = pwm[0];
	tpwm[1] = pwm[1];
	tpwm[2] = pwm[2];
	tpwm[3] = pwm[3];
	tpwm[4] = pwm[4];
	tpwm[5] = pwm[5];

}

//�ı�PWMֵ
void SetPwm(u8 p1,u8 p2, u8 p3, u8 p4, u8 p5,u8 p6)
{	
	/*
	pwm[0] = p1;
	pwm[1] = p2;
	pwm[2] = p3;
	pwm[3] = p4;
	pwm[4] = p5;
	pwm[5] = p6;
	*/
//	bUpdataPwm = TRUE;


	if (p1)pwm[0] = p1+MIN_VALUE;  //PWMֵ��С��2����ֹ����
	else pwm[0] = 0;

	if (p2)pwm[1] = p2+MIN_VALUE;
	else pwm[1] = 0;

	if (p3)pwm[2] = p3+MIN_VALUE;
	else pwm[2] = 0;

	if (p4)pwm[3] = p4+MIN_VALUE;
	else pwm[3] = 0;

	if (p5)pwm[4] = p5+MIN_VALUE;
	else pwm[4] = 0;

	if (p6)pwm[5] = p6+MIN_VALUE;
	else pwm[5] = 0;

}

//�Խṹ����ʽ����PWM
//mode ;0:������ʽ ,1:ֱ����
void SetPwmB(STRUCT_PWM *pPwm,u8 mode)
{
	if (mode)
	{
		SetPwmA(
			pPwm->pwm1,
			pPwm->pwm2,
			pPwm->pwm3,
			pPwm->pwm4,
			pPwm->pwm5,
			pPwm->pwm6
			);
	}
	else
	{
		SetPwm(
			pPwm->pwm1,
			pPwm->pwm2,
			pPwm->pwm3,
			pPwm->pwm4,
			pPwm->pwm5,
			pPwm->pwm6
			);
	}
}


//�Խṹ����ʽ���ص�ǰPWM
STRUCT_PWM *GetPwmB(void)
{
	static STRUCT_PWM tpwm;
	tpwm.pwm1 = pwm[0];
	tpwm.pwm2 = pwm[1];
	tpwm.pwm3 = pwm[2];
	tpwm.pwm4 = pwm[3];
	tpwm.pwm5 = pwm[4];
	tpwm.pwm6 = pwm[5];
	return &tpwm;
}

//����PWMֵ
u8 UpdatePWM(void)
{
	bIsUpdate = FALSE;
	if (pwm[0] != tpwm[0])
	{
		if (pwm[0] > tpwm[0])tpwm[0]++;
		else tpwm[0]--;
		SET_PWM1(tpwm[0]);
		bIsUpdate = TRUE;
	}

	if (pwm[1] != tpwm[1])
	{
		if (pwm[1] > tpwm[1])tpwm[1]++;
		else tpwm[1]--;
		SET_PWM2(tpwm[1]);
		bIsUpdate = TRUE;
	}

	if (pwm[2] != tpwm[2])
	{
		if (pwm[2] > tpwm[2])tpwm[2]++;
		else tpwm[2]--;
		SET_PWM3(tpwm[2]);
		bIsUpdate = TRUE;
	}

	if (pwm[3] != tpwm[3])
	{
		if (pwm[3] > tpwm[3])tpwm[3]++;
		else tpwm[3]--;
		SET_PWM4(tpwm[3]);
		bIsUpdate = TRUE;
	}

	if (pwm[4] != tpwm[4])
	{
		if (pwm[4] > tpwm[4])tpwm[4]++;
		else tpwm[4]--;
		SET_PWM5(tpwm[4]);
		bIsUpdate = TRUE;
	}

	if (pwm[5] != tpwm[5])
	{
		if (pwm[5] > tpwm[5])tpwm[5]++;
		else tpwm[5]--;
		SET_PWM6(tpwm[5]);
		bIsUpdate = TRUE;
	}

	return bIsUpdate;
}

//��ȡָ��ͨ��������ֵ ch = 1~6 
u8	GetPwm(u8 ch)
{
	if (ch <= PWM_NUM)
	{
		if (pwm[ch-1])return pwm[ch-1]-2;
		else return 0;
	}
	else
	{
		return 0;
	}
}

//��ȡPWM��ǰ�Ƿ��б仯��0:�ޱ仯,!0�б仯
u8 GetPwmUpataState(void)
{
	return bIsUpdate;
}

