/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 定时计算
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/
#include "TimerFunc.h"
#include "func.h"
#include "rtc.h"
#include "system.h"
#include "stmflash.h"
#include "my_type.h"
#include "timer.h"
#include "rtx_os.h"

u8 GetMaxTimerStep(void);

//计算当前的亮度
//实现两个时间点之间渐变的效果
//算法：计算当前时间在两个时间点之前的百分比，通过时间百分比计算亮度值。
void SetPwmByTimer(u8 hour,u8 minute)
{
	u16 t1,t2,t_3,t_total; //t1时间点1;t2时间点2;t3：t2和t1的时间差
	u16 tc;  //当前时间
	float percent; //百分比
	u8 t_pwm;
	u8 c_pwm;

	u8 pwmX[PWM_NUM];
	u8 *p1,*p2;
	u8 i;

	sysInfo.indexTimer = findCurrectPwm(hour,minute); //找到当前定时点
	//sysInfo.nextTimer = (sysInfo.indexTimer +1)%PRGM_STEP; //下一个时间点
	sysInfo.nextTimer = (sysInfo.indexTimer +1)% GetMaxTimerStep(); //下一个时间点

	t1 = TimerProg[sysInfo.indexTimer ].hour*60+TimerProg[sysInfo.indexTimer].minute; //时间点1
	t2 = TimerProg[sysInfo.nextTimer].hour*60+TimerProg[sysInfo.nextTimer].minute; //时间点2
	tc = hour*60+minute; //当前时间
	
	if (tc < t1)tc += 1440; //到了第二天

	if (t2 < t1)t2 += 1440; //t2比t1小，循环到第二天 24*60=1440分;
			
	t_3 = tc - t1; //当前时间到定时1的长度

	t_total = t2 - t1; //两个定时点的长度
	percent = (float)t_3 / (float)t_total;//计算当前时间点间隔的百分比

	//计算PWM1~PWM6的值
	p1 = &TimerProg[sysInfo.indexTimer].pwm1;
	p2 = &TimerProg[sysInfo.nextTimer].pwm1;
	for (i=0;i<PWM_NUM;i++) 
	{
		if (*p2 >= *p1)
		{
			t_pwm = *p2 - *p1;
			c_pwm = *p1+ (char)(percent*t_pwm);
		}
		else
		{
			t_pwm = *p1 - *p2;
			c_pwm = *p1- (char)(percent*t_pwm);
		}
		pwmX[i] = c_pwm;
		p1++;
		p2++;
	}

	//置当前的亮度
	SetPwm(
		pwmX[0],
		pwmX[1],
		pwmX[2],
		pwmX[3],
		pwmX[4],
		pwmX[5]
		);
}

/********************************************************************************** 
 * Function Name  : void findCurrectPwm(void)
 * Description    : 查找当前时间PWM预设值,时间循环方式 lfx
 * Input&
 * Output   : 返回当前的定时序号
 ********************************************************************************/
u8 findCurrectPwm(u8 hour,u8 minute)
{
	u16  timeNow;	//当前时间
	u16	 timePwm;
	u8  i,post;	
	BOOL IsFind = FALSE;	//找到比当前时间小的标志

	timeNow = hour*HOUR_RADIX+minute; //计算当前时间
	STM_FLASH_READ(FLASH_TIMER_ADDR,(u16*)TimerProg,sizeof(TimerProg));//读取PWM信息

	//计算当前时间在已排序时间中的位置
	IsFind = FALSE;  
	for(i=0;i<PRGM_STEP;i++)
	{
		if (TimerProg[i].enable)// != DISABLE
		{
			timePwm = TimerProg[i].hour*HOUR_RADIX + TimerProg[i].minute ; //计算时间
			if((timeNow > timePwm))//时间相等并且要为开启状态
			{
				IsFind = TRUE;
				post = i;	//记录位置,总是找到最大的那位置
			}
		}
		else
		{
			break; //跳出循环
		}
	}

	if(!IsFind)//没有找到
	{
	//	post = PRGM_STEP - 1;	//取最后一个时间点的亮度 
		post = GetMaxTimerStep()-1;  //注意：定时时段必段大于等于1

	}
	return post;
}

//取最大的定时点
u8 GetMaxTimerStep(void)
{
	u8 setp;
	u8 i;

	setp = 0;
	for (i=0;i<PRGM_STEP;i++)
	{
		if (TimerProg[i].enable) //定时开启，就加1
		{
			setp++;
		}
		else  
		{
			break;  //跳出循环
		}
	}
	
	return setp;
}

//内存复制
//source 源地址
//target 目标地址
void TimerCopy(TIMER* target,TIMER* source)
{
	u8 size = sizeof(TIMER);
	u8* ps = (u8*)source;
	u8* pt = (u8*)target;
	while(size--)
	{
		*pt = *ps;
		pt++;
		ps++;
	}
}
/*---------------lfx---------------*/
//排序定时时间
void SortTimer(void)
{
	u8 i,j;//m;
	u16 time1,time2;
	TIMER temp;

	for(i=PRGM_STEP-1;i!=0;i--)
	{
		for (j = 0;j < i; j++)
		{
			time1 = TimerProg[j].hour * HOUR_RADIX + TimerProg[j].minute;
			time2 = TimerProg[j+1].hour * HOUR_RADIX + TimerProg[j+1].minute;
			if(time1 > time2)
			{
				//交换数据
				TimerCopy(&temp,&TimerProg[j]);
				TimerCopy(&TimerProg[j],&TimerProg[j+1]);
				TimerCopy(&TimerProg[j+1],&temp);
			}
		}
	}
}


//-----------end of file--------------

