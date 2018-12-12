/*******************************************************
ULN2003驱动5V减速步进电机程序
Target:STC89C52RC-40C
Crystal:12MHz
Author:战神单片机工作室
Platform:51&avr单片机最小系统板+ULN2003步进电机驱动套件
http://zsmcu.taobao.com   QQ:284083167
*******************************************************
转一周需要的步数:
//四相八拍需要4096个脉冲一圈 而四相四拍只要一半。 360 / 5.625 / 64 = 步距角
接线方式:
IN1 ---- P00
IN2 ---- P01
IN3 ---- P02
IN4 ---- P03
+   ---- +5V
-   ---- GND
*********************/

#include <math.h>
#include "sys.h"
#include "delay.h"
#include "uln2003.h"
#include "rtl.h"
#include "func.h"
#include "rtc.h"


const u8 phasecw[8] = { 0x08, 0x0c, 0x04, 0x06,0x02,0x03,0x01,0x09 };//正转   电机导通相序 A-AB-B-BC-C-CD-D-DA
u8 phase_falg = 0;

void TIM1_Int_Init(u16 arr,u16 psc);
//停止转动
void MotorStop(void)
{
	TIM_Cmd(TIM1, DISABLE);
	MotorA = 0;
	MotorB = 0;
	MotorC = 0;
	MotorD = 0;
	
}

void MotorInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);        //JTAG-DP 失能 + SW-DP使能 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_8);
	
	TIM1_Int_Init(1300,72-1);
}

//顺时针转动
//正转   电机导通相序 A-AB-B-BC-C-CD-D-DA
void MotorStart(uint8_t flag,u16 arr)
{
	phase_falg = flag;
	TIM1_Int_Init(arr,72-1);
	TIM_Cmd(TIM1, ENABLE);
}

#if 0
//匀速运转
void MotoTest1(void)
{

}

//加速运转
void MotoTest2(void)
{

}

//电机功能执行
void MotorRunPresse(void)
{

	
}
#endif
//查找电机在当前时间是否有效
u8 MotorIsEffective(u8 cur_hour, u8 cur_min)
{
	u16  timeNow;	//当前时间
//	u8 motorSpeed;		//多少秒执行一步
	u16 timeStar, timeEnd;

	timeNow = cur_hour * 60 + cur_min; //读取系统时间


	if (motorInfo.enable == 1)
	{

		//第一个时间段
		timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
		timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
		if (timeEnd >= timeStar)		//在同一天时间里
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}
		else		//不在同一天时间里
		{
	
			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}

		//第二个之间段
		timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
		timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
		if(timeEnd >= timeStar)		//在同一天时间里
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}
		else		//不在同一天时间里
		{

			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}


		//第三个之间段
		timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
		timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
		if (timeEnd >= timeStar)		//在同一天时间里
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}
		else		//不在同一天时间里
		{

			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
			{
				return 1;
			}
		}
	}
	return 0;
}

//电机旋转总时间
uint32_t GetMotorRunTimer(void)
{
	
	u16 timeStar, timeEnd;
	uint32_t timeSum = 0;
	uint32_t current_time;		//当前系统时间

	
	#if 1
	//第一个时间段
	timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	
	//第二个之间段
	timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	
	//第三个之间段
	timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	timeSum = timeSum * 60;
	
	#else
	current_time = calendar.hour*60+calendar.min;		//获取当前分钟时刻
		//第一个时间段
	timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	
		//第二个之间段
	timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	
		//第三个之间段
	timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	timeSum = timeSum * 60;
	#endif
	return timeSum;
}

//电机旋转总步数
uint32_t  GetMotorRunSetp(void)
{
	uint16_t angle;					//电机旋转总角度
	uint16_t motor_step_sum = 0;	//电机应该旋转的步数
	
	angle = motorInfo.angle_h*100+motorInfo.angle_l;
	motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//计算电机需要旋转的步数(电机动一下5步)
	return  motor_step_sum;
}


//获取电机执行一次的间隔秒数

//电机自动返回原点
void MotorAutoGoBack(_calendar_obj time)
{
	if((time.hour == 23)&&(time.min == 59)&&(time.sec > 55))
	{
		//SEGGER_RTT_printf(0,"MotorAutoGoBack\r\n");
		MotorGoBack();
	}
}
//上电回到原点
void MotorBackOrigin(void)
{

}


void TIM1_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //①时钟 TIM1 使能
	//定时器 TIM1 初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0; //高级定时器1是用定时器功能配置这个才可以是正常的计数频率一开始的72mhz 值得注意的地方
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //②初始化 TIM1
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //③允许更新中断
	//中断优先级 NVIC 设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //TIM1 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 1 级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级 3 级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道被使能
	NVIC_Init(&NVIC_InitStructure); //④初始化 NVIC 寄存器
	TIM_Cmd(TIM1, ENABLE); //⑤使能 TIM1

}

static uint8_t index=0;
static u16 tim1_arr = 1500;
u16 motor_current_step = 0;
u16 motor_set_step = 0;
static uint8_t go_back_flag = 0;
static uint8_t step_1;

void TIM1_UP_IRQHandler(void) //TIM1 中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查 TIM1 更新中断发生与否
	{
		TIM_ClearFlag(TIM1, TIM_IT_Update ); //清除 TIM1 更新中断标志
		if(phase_falg == 0)
		{
			MotorA = (phasecw[index]&0x01?1:0);	
			MotorB = (phasecw[index]&0x02?1:0);	
			MotorC = (phasecw[index]&0x04?1:0);	
			MotorD = (phasecw[index]&0x08?1:0);
			index ++;
			if(index==8)
			{
				index=0;
				if(motor_current_step > 0) 
				{
					motor_current_step--;
				}
				
			}
		}
		else
		{
			MotorD = (phasecw[index]&0x01?1:0);	
			MotorC = (phasecw[index]&0x02?1:0);	
			MotorB = (phasecw[index]&0x04?1:0);	
			MotorA = (phasecw[index]&0x08?1:0);
			index ++;
			if(index==8)
			{
				index=0;
				if(motor_current_step<=LED_MOTOR_STEP_MAX)
				{
					motor_current_step++;
				}
			}
		}	
		if(motor_current_step == motor_set_step)
		{
			MotorStop();
		}
//		index ++;
//		if(index==8)
//		{
//			index=0;
//			motor_current_step++;
//			if(go_back_flag==0)
//			{
//				if(step_1 > motorInfo.speed )			//一次15步
//				{
//					step_1 = 0;
//					MotorStop();
//				}
//				step_1++;
//			}
//		}
	}
}



//快速回转
void MotorGoBack(void)
{
	u8 i = 0;
//	go_back_flag = 1;
//	for(i=0;i<15;i++)
//	{
//		MotorStart(0,2000 - (i*100));
//		os_dly_wait(50);
//	}
//	MotorStart(0,1500);
	motor_current_step = LED_MOTOR_STEP_MAX;
	SetMotoCurrentStep(0);
	while(1)
	{
//		if(motor_current_step > step)
//		{
//			MotorStop();
//			go_back_flag = 0;
//			motor_current_step  = 0;
//			break;
//		}
//		SEGGER_RTT_printf(0,"MotorGoBack = %d\r\n",motor_current_step);
		MotorStepRefresh();
		if(motor_current_step == 0)
		{
		   break;
		}
		os_dly_wait(100);
//		os_tsk_pass();
	}
}

//快速前转
void MotorGoTo(uint16_t step)
{
	go_back_flag = 1;		//快速转动标记
	MotorStart(1,1300);
	//MotorStart(1,300);
	while(1)
	{
		if(motor_current_step > step)
		{

			MotorStop();
			go_back_flag = 0;
			break;
		}
		//SEGGER_RTT_printf(0,"MotorGoTo = %d\r\n",motor_current_step);
		os_dly_wait(20);
		os_tsk_pass();
	}
}


//电机安照设定速度转动
//speed:速度
//转动方向
void MotorRun(uint16_t speed,uint8_t dir)
{
	go_back_flag = 1;
	MotorStart(dir,speed);
	
}
//电机走到当前时间应该到达的地方
uint16_t MotorGotoCurrentPoint(uint16_t step_sum,_calendar_obj calendar,MotorInfo_s motor_info)
{
	uint16_t over_time;		//已经过去的时间（单位是分钟）
	uint16_t set_time1,set_time2,set_time3;		//电机设定的时间段
	u16 timeStar1, timeEnd1,timeStar2, timeEnd2,timeStar3, timeEnd3;		//电机设置的时间开始，时间结束
	u16 current_time;

	
	//第一个时间段
	timeStar1 = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd1 = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	set_time1 = timeEnd1 - timeStar1;
	
	//第二个之间段
	timeStar2 = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd2 = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	set_time2 = timeEnd2 - timeStar2;
	
	//第三个之间段
	timeStar3 = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd3 = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	set_time3 = timeEnd3 - timeStar3;
	
	//当前时间(单位是分钟)
	current_time = calendar.hour*60+calendar.min;
	
	if(current_time<=timeStar1)		//当前时间没有在设定的时间内，说明还没有错过旋转时间
	{
		over_time = 0;
	}
	else if(current_time<=timeEnd1)		//当前时间在第一段时间内
	{
		over_time = current_time-timeStar1;		//错过的旋转时间时长
	}
	else if(current_time<=timeStar2)				//当前时间已经过了第一段时间，还没到第二段时间
	{
		over_time = set_time1;
	}
	else if(current_time<=timeEnd2)			//在第二段时间内
	{
		over_time = current_time-timeStar2+set_time1;		//错过的旋转时间时长
	}
	else if(current_time<=timeStar3)			//已经过了前面两段时间，但是没到第三段时间
	{
		over_time = set_time1+set_time2;
	}
	else if(current_time<=timeEnd3)		//在第三段时间内
	{
		over_time = current_time-timeStar3+set_time1+set_time2;
	}
	return over_time;
}

void MotorRest(void)
{

	uint16_t angle;					//电机旋转总角度
	uint32_t motor_run_time =0;		//电机旋转总时间
	uint16_t motor_step_sum = 0;	//电机应该旋转的步数
	uint16_t interval_sec = 0;		//间隔执行一次电机旋转变量
	uint16_t over_time;				//电机应该旋转经过的时间(分钟)
	uint16_t current_step_1;		//当前应该到达的步数
	
	MotorGoBack();//电机返回原点
	over_time = MotorGotoCurrentPoint(LED_MOTOR_STEP_MAX,calendar,motorInfo);
	motor_run_time = GetMotorRunTimer();	//获取电机时间
	angle = motorInfo.angle_h*100+motorInfo.angle_l;	
	motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//计算电机需要旋转的步数(电机动一下5步)
	interval_sec = motor_run_time/(motor_step_sum/5);		//执行一次5步
	current_step_1 = motor_step_sum*over_time*60/motor_run_time;
	MotorGoTo(current_step_1);			//快速到达当前时间应该到的角度
}


uint16_t GetMotorCurrentStep(void)
{
	return motor_current_step;
}

uint16_t SetMotoCurrentStep(uint16_t step)
{
	motor_set_step = step;
}

void MotorStepRefresh(void)
{
    uint8_t step;	//设置和当前步数相差步数
	
	//step = fabs(motor_current_step-motor_set_step);  //求绝对值
	
	//if(step > 5)   //设置和当前步数相差5步才执行电机旋转
	//{
		if(motor_current_step > motor_set_step)			//当前步数大于设置的步数，那么要反转
		{
			MotorStart(0,2000);			//反转
		}
		else if(motor_current_step < motor_set_step)
		{
			MotorStart(1,2000);			//正转
		}
	//}
}


//当前时间错过的秒数
u32 GetMotorMissSec(_calendar_obj time)
{
	uint32_t over_time = 0;		//已经过去的时间（单位是分钟）
	uint16_t set_time1,set_time2,set_time3;		//电机设定的时间段
	u16 timeStar1, timeEnd1,timeStar2, timeEnd2,timeStar3, timeEnd3;		//电机设置的时间开始，时间结束
	u16 current_time;

	
	//第一个时间段
	timeStar1 = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd1 = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	set_time1 = timeEnd1 - timeStar1;
	
	//第二个之间段
	timeStar2 = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd2 = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	set_time2 = timeEnd2 - timeStar2;
	
	//第三个之间段
	timeStar3 = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd3 = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	set_time3 = timeEnd3 - timeStar3;
	
	//当前时间(单位是分钟)
	current_time = calendar.hour*60+calendar.min;
	
	if(current_time<timeStar1)		//当前时间没有在设定的时间内，说明还没有错过旋转时间
	{
		over_time = 0;
	}
	else if(current_time<=timeEnd1)		//当前时间在第一段时间内
	{
		over_time = current_time-timeStar1;		//错过的旋转时间时长
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//换算成秒单位,加上当前的秒时钟
	}
	else if(current_time<timeStar2)				//当前时间已经过了第一段时间，还没到第二段时间
	{
		over_time = set_time1;
		over_time =  over_time*60;		//换算成秒单位
	}
	else if(current_time<=timeEnd2)			//在第二段时间内
	{
		over_time = current_time-timeStar2+set_time1;		//错过的旋转时间时长
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//换算成秒单位,加上当前的秒时钟
	}
	else if(current_time<timeStar3)			//已经过了前面两段时间，但是没到第三段时间
	{
		over_time = set_time1+set_time2;
		over_time =  over_time*60;		//换算成秒单位
	}
	else if(current_time<=timeEnd3)		//在第三段时间内
	{
		over_time = current_time-timeStar3+set_time1+set_time2;
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//换算成秒单位,加上当前的秒时钟
	}
	return over_time;
}

//当前时间电机应该走过的步数

u16 GetMotorMissStep(void)
{
	u16 RunSetp = 0;  //电机运行的总步数
	u16 RunSec = 0;		//总时间
	u16 set_step = 0;	//当前时间应该设置的步数
	u16 miss_sec = 0;
	
	RunSetp = GetMotorRunSetp();
	RunSec = GetMotorRunTimer();
	miss_sec = GetMotorMissSec(calendar);
	
	set_step =  (miss_sec* RunSetp)/RunSec;
	
	return set_step;
}
