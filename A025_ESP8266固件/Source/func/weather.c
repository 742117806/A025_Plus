/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: weather.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 灯光效果
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "weather.h"
#include "timer.h"
#include "stmflash.h"
#include "system.h"
#include "func.h"
#include "rtc.h"
#include "pwmData.h"

WEATHER WeatherProg[WEATHER_SETP]; //天气

u8 moonLight_leve;		//月相的强度

//天气默认数据
const WEATHER weather_data[WEATHER_SETP]={
	{0,WEATHER_TYPE_FLASH,10,05,10,10,30,0},
	{0,WEATHER_TYPE_CLUED,11,05,11,10,20,0},
	{0,WEATHER_TYPE_FLASH,11,13,11,16,50,0},
	{0,WEATHER_TYPE_CLUED,18,05,18,10,3,0}
};

//月相默认数据
const MOONLING_s moonling_data[28] = {
	{ 1, 1, 19, 0, 6, 0, 5 ,0 },
	{ 1, 2, 19, 0, 6, 0, 6, 0 },
	{ 1, 3, 19, 0, 6, 0, 7, 0 },
	{ 1, 4, 19, 0, 6, 0, 8, 0 },
	{ 1, 5, 19, 0, 6, 0, 9, 0 },
	{ 1, 6, 19, 0, 6, 0, 10, 0 },
	{ 1, 7, 19, 0, 6, 0, 11, 0 },
	{ 1, 8, 19, 0, 6, 0, 12, 0 },
	{ 1, 9, 19, 0, 6, 0, 13, 0 },
	{ 1, 10, 19, 0, 6, 0, 14, 0 },
	{ 1, 11, 19, 0, 6, 0, 15, 0 },
	{ 1, 12, 19, 0, 6, 0, 16, 0 },
	{ 1, 13, 19, 0, 6, 0, 17, 0 },
	{ 1, 14, 19, 0, 6, 0, 18, 0 },
	{ 1, 15, 19, 0, 6, 0, 19, 0 },
	{ 1, 16, 19, 0, 6, 0, 18, 0 },
	{ 1, 17, 19, 0, 6, 0, 17, 0 },
	{ 1, 18, 19, 0, 6, 0, 16, 0 },
	{ 1, 19, 19, 0, 6, 0, 15, 0 },
	{ 1, 20, 19, 0, 6, 0, 14, 0 },
	{ 1, 21, 19, 0, 6, 0, 13, 0 },
	{ 1, 22, 19, 0, 6, 0, 12, 0 },
	{ 1, 23, 19, 0, 6, 0, 11, 0 },
	{ 1, 24, 19, 0, 6, 0, 10, 0 },
	{ 1, 26, 19, 0, 6, 0, 9, 0 },
	{ 1, 26, 19, 0, 6, 0, 8, 0 },
	{ 1, 27, 19, 0, 6, 0, 7, 0 },
	{ 1, 28, 19, 0, 6, 0, 6, 0 },
};

//云彩效果数据
#define CLUED_NUM	7
const STRUCT_PWM cluedEffectData[CLUED_NUM]={
	{100,1,1,1,1,1},
	{1,100,1,1,1,1},
	{1,1,20,100,100,100},
	{100,1,1,1,100,100},
	{1,1,100,100,100,100},
	{1,1,20,20,1,100},
	{1,1,20,20,100,1},
};

//闪电效果数据
#define FLASH_SPPED		400  //闪电的速度
#define FLASH_PWM		5  //最低亮度
#define FLASH_MIN_PWM	20  
#define FLASH_TYPE_NUM		4
const STRUCT_PWM flashEffectData[FLASH_TYPE_NUM]={
	{100,80,20,5,5,5},
	{20,100,20,5,5,5},
	{20,50,100,5,5,5},
	{100,100,100,5,5,5}
};
const STRUCT_PWM flashOndata = {5,5,5,5,5,5};

#define RANNUM	6
const u8 RanFlashNum[RANNUM]={0,1,1,1,2,3}; //随机闪的数量

float flash_scale;  //闪电强弱比例因子
u16 flash_cnt;  //时长计数

static u8 cntOff = 0;
static u8 flash_num = 0; //连续闪次数

void (*EffectFunc)(void); //要执行的灯具效果

//空函数，无效果
void NullEffect(void){}

//查找当前灯光效果
u8 findCurrectEffect(void)
{
	u16  timeNow;	//当前时间
	u16	 timeStar,timeEnd;
	u8 i;
	u8 bFindEffect; //找到

	STM_FLASH_READ(FLASH_EFFECT_ADDR,(u16*)WeatherProg,sizeof(WeatherProg));//读取PWM信息
	timeNow = calendar.hour*60 + calendar.min; //读取系统时间
	
	bFindEffect = 0;
	for (i=0;i<WEATHER_SETP;i++)
	{
		if (WeatherProg[i].enable)
		{
			timeStar = WeatherProg[i].s_hour*60 + WeatherProg[i].s_minute;
			timeEnd = WeatherProg[i].e_hour*60 + WeatherProg[i].e_minute;
			if (timeNow >= timeStar && timeNow <= timeEnd) //在开始和结束时间之间
			{
				bFindEffect = 1;
				switch(WeatherProg[i].effect)
				{
				case WEATHER_TYPE_CLUED:  //云彩模式
					if(sysInfo.workmode != MODE_CLUDE && sysInfo.workmode !=MODE_CLUDE_VEIW)
					{
						InitCloudEffect(WeatherProg[i].level);
					}
					break;

				case WEATHER_TYPE_FLASH:  //闪电模式
					if(sysInfo.workmode != MODE_FLASH && sysInfo.workmode !=MODE_FLASH_VEIW)
					{
						InitFlashEffect(WeatherProg[i].level);
					}
					break;
				}
			}
		}
	}
	
	if (!bFindEffect)
	{
		DeleteEffect();
	}
	return bFindEffect;
}

//查找月相功能
//u8 findMoonLightEffect(void)
//{
//	u16  timeNow;	//当前时间
//	u16	 timeStar, timeEnd;
//	u8 bFindEffect = 0; //找到 1表示找到
//	MOONLING_s MoonlignProg;
//
//	timeNow = calendar.hour * 60 + calendar.min; //读取系统时间
//	STM_FLASH_READ(FLASH_MOONLIGHT_ADDR + (calendar.w_date - 1)*(sizeof(MOONLING_s)), (u16*)&MoonlignProg, sizeof(MoonlignProg));//读取PWM信息
//	timeStar = MoonlignProg.s_hour * 60 + MoonlignProg.s_minute;		//月相开始时间
//	timeEnd = MoonlignProg.e_hour * 60 + MoonlignProg.e_minute;			//月相结束时间
//	if (MoonlignProg.enable == 1)
//	{
//		if (timeEnd >= timeStar)		//在同一天时间里
//		{
//			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
//			{
//				bFindEffect = 1;
//				InitMoonLightEffect(MoonlignProg.level);
//			}
//		}
//		else		//不在同一天时间里
//		{
//
//			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//当前时间在月相开始和结束时间之间
//			{
//				bFindEffect = 1;
//				InitMoonLightEffect(MoonlignProg.level);
//			}
//		}
//	}
//	return bFindEffect;
//}
//初始化云彩
void InitCloudEffect(u8 level)
{
	level = 100 - level;
	if (level)SetPwmSpeed(level); //设置速度
	else SetPwmSpeed(1); //默认速度为2
	SetEffectFun(CloudEffect);
	SetWorkMode(MODE_CLUDE);
}

//初始化闪电
void InitFlashEffect(u8 level)
{
	if (!level)level = 1;

	flash_scale =(float)level / 100.0;

	cntOff = 0;
	flash_num = 0;
	flash_cnt = 0;

	SetPwmSpeed(PWM_SPEED_FLASH);
	SetEffectFun(FlashEffect);
	SetWorkMode(MODE_FLASH);
}

//初始化月相效果
void InitMoonLightEffect(u8 Level)
{
	moonLight_leve = Level;
	SetPwmSpeed(PWM_SPEED_FLASH);
	SetEffectFun(MoonLingEffect);
	SetWorkMode(MODE_MOONLIGHT);
}

//删除特效
void DeleteEffect(void)
{
	SetWorkMode(MODE_TIMER);
	//EffectFunc = &NullEffect;
	SetEffectFun(NullEffect);
}

//云彩效果
void CloudEffect(void)
{
	static u8 i=0;
	u8 pwmstate;

	pwmstate = GetPwmUpataState();

	if (!pwmstate)
	{
		SetPwm(cluedEffectData[i].pwm1,
				cluedEffectData[i].pwm2,
				cluedEffectData[i].pwm3,
				cluedEffectData[i].pwm4,
				cluedEffectData[i].pwm5,
				cluedEffectData[i].pwm6
				);
		if (++i >= CLUED_NUM)i=0;
	}
}

//月相效果
void MoonLingEffect(void)
{
//	static u8 i = 0;
	u8 pwmstate;

	pwmstate = GetPwmUpataState();

	if (!pwmstate)
	{
		SetPwm(moonLight_leve,
			moonLight_leve,
			moonLight_leve,
			moonLight_leve,
			moonLight_leve,
			moonLight_leve
			);
	}
}

//闪电效果
void FlashEffect(void)
{
	u8 type; //闪烁类型 
	u8 pwmX[3];
	if (!flash_cnt)
	{
		if (!flash_num) //闪完了
		{
			flash_cnt = Random(20,FLASH_SPPED);
			flash_num = RanFlashNum[Random(0,RANNUM-1)]; //连续闪的次数
		}
		else
		{
			flash_num--;
			flash_cnt = 20;
		}

		type = Random(0,FLASH_TYPE_NUM-1); //闪电类型

		pwmX[0] =(float)(flashEffectData[type].pwm1- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM; //算法：(高度－最低亮度)*比例+最低亮度
		pwmX[1] =(float)(flashEffectData[type].pwm2- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM;
		pwmX[2] =(float)(flashEffectData[type].pwm3- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM;

		SetPwmA(pwmX[0],
				pwmX[1],
				pwmX[2],
				FLASH_PWM,
				FLASH_PWM,
				FLASH_PWM
			);
		cntOff = Random(2,10); //持继时间
	}
	else
	{
		flash_cnt--;
	}

	//回到正常亮度
	if (!cntOff)
	{
		SetPwmA(flashOndata.pwm1,
				flashOndata.pwm2,
				flashOndata.pwm3,
				flashOndata.pwm4,
				flashOndata.pwm5,
				flashOndata.pwm6			
			); //亮起
	}
	else
	{
		cntOff--;
	}
}

//取随机数 ,此函数有一定局限性
u16 Random(u16 min,u16 max)
{
	u16 temp;
	temp = SysTick->VAL;  //读TIM3的计数器
	temp = temp % (max+1);
	if (temp < min)
	{
		temp = min;
	}
	return temp;
}

//取最大的定时点
u8 GetMaxEffectStep(void)
{
	u8 setp;
	u8 i;

	STM_FLASH_READ(FLASH_EFFECT_ADDR,(u16*)WeatherProg,sizeof(WeatherProg));//读取PWM信息

	setp = 0;
	for (i=0;i<WEATHER_SETP;i++)
	{
		if (WeatherProg[i].enable) //定时开启，就加1
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

//---------end of file-------------
