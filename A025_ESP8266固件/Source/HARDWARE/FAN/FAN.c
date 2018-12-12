/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: fan.c
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-10
@Description: 风扇调速功能
@Function List: 
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/10 1.0 
***********************************************************/
#include "fan.h"
#include "stm32f10x_tim.h"

u8 fanspeed;  //风扇转速

#define  SET_FAN_SPEED(X)	TIM_SetCompare2(TIM2,X);

#define FAN_MAX_PWM		100		//最大PWM值
#define FAN_PWM_SPEED	20000		//20K
#define FAN_SYS_CLK		72000000

//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

	//设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

	//初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM4 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
//	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 
//	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 
//	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 

//	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
//	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_Cmd(TIM2, ENABLE);  //使能TIM4
}

//风扇初始化
void FAN_init(void)
{
	TIM2_PWM_Init(FAN_MAX_PWM,(FAN_SYS_CLK/FAN_MAX_PWM/FAN_PWM_SPEED));
	SetFanSpeed(0);
}

//设置风扇转速
void SetFanSpeed(u8 speed)
{
	if (speed > 100)speed = 100; //最大只有100

	fanspeed = speed;
	SET_FAN_SPEED(fanspeed);
}

//获取风扇转速
u8	GetFanSpeed(void)
{
	return fanspeed;
}


