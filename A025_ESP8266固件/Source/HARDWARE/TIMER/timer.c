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
#include "timer.h"
#include "stm32f10x_tim.h"

//V1.1 20140904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
#define  SET_PWM1(X)	TIM_SetCompare1(TIM4,X)
#define  SET_PWM2(X)	TIM_SetCompare2(TIM4,X)
#define  SET_PWM3(X)	TIM_SetCompare3(TIM4,X)
#define  SET_PWM4(X)	TIM_SetCompare4(TIM4,X)
#define  SET_PWM5(X)	TIM_SetCompare1(TIM3,X)
#define  SET_PWM6(X)	TIM_SetCompare2(TIM3,X)

void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);

#define MIN_VALUE	2 //PWM最小值

u16 tpwm[PWM_NUM]= {0,0,0,0,0,0};
u16 pwm[PWM_NUM]= {0,0,0,0,0,0};

u8 bIsUpdate = FALSE;  //是否正在更新PWM

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);        //TIM3映射到PB口，部分重映射 

   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOC.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}


//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM4, ENABLE); //Timer3部分重映射  TIM4_CH2->PB5    

	//设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	//初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM4 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
}

//初始化6路PWM
void PWM_Init(void)
{
	TIM3_PWM_Init(MAX_PWM,(SYS_CLK/MAX_PWM/PWM_SPEED));//3	 //PSC值=系统频率/PWM阶级/PWM频率
	TIM4_PWM_Init(MAX_PWM,(SYS_CLK/MAX_PWM/PWM_SPEED)); //3

	//PWM值设为0
	SET_PWM1(0);
	SET_PWM2(0);
	SET_PWM3(0);
	SET_PWM4(0);
	SET_PWM5(0);
	SET_PWM6(0);
}

//直接设置亮度
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
	//PWM值最小是2，防止灯闪
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
	//保证读取出来的PWM值一致
	SetPwm(p1,p2,p3,p4,p5,p6);
	/*
	if (p1)pwm[0] = p1+MIN_VALUE;  //PWM值最小是2，防止灯闪
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

//改变PWM值
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


	if (p1)pwm[0] = p1+MIN_VALUE;  //PWM值最小是2，防止灯闪
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

//以结构体形式设置PWM
//mode ;0:渐变形式 ,1:直接亮
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


//以结构体形式返回当前PWM
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

//更新PWM值
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

//获取指定通道的亮度值 ch = 1~6 
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

//获取PWM当前是否有变化，0:无变化,!0有变化
u8 GetPwmUpataState(void)
{
	return bIsUpdate;
}

