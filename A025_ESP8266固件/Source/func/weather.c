/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: weather.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �ƹ�Ч��
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

WEATHER WeatherProg[WEATHER_SETP]; //����

u8 moonLight_leve;		//�����ǿ��

//����Ĭ������
const WEATHER weather_data[WEATHER_SETP]={
	{0,WEATHER_TYPE_FLASH,10,05,10,10,30,0},
	{0,WEATHER_TYPE_CLUED,11,05,11,10,20,0},
	{0,WEATHER_TYPE_FLASH,11,13,11,16,50,0},
	{0,WEATHER_TYPE_CLUED,18,05,18,10,3,0}
};

//����Ĭ������
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

//�Ʋ�Ч������
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

//����Ч������
#define FLASH_SPPED		400  //������ٶ�
#define FLASH_PWM		5  //�������
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
const u8 RanFlashNum[RANNUM]={0,1,1,1,2,3}; //�����������

float flash_scale;  //����ǿ����������
u16 flash_cnt;  //ʱ������

static u8 cntOff = 0;
static u8 flash_num = 0; //����������

void (*EffectFunc)(void); //Ҫִ�еĵƾ�Ч��

//�պ�������Ч��
void NullEffect(void){}

//���ҵ�ǰ�ƹ�Ч��
u8 findCurrectEffect(void)
{
	u16  timeNow;	//��ǰʱ��
	u16	 timeStar,timeEnd;
	u8 i;
	u8 bFindEffect; //�ҵ�

	STM_FLASH_READ(FLASH_EFFECT_ADDR,(u16*)WeatherProg,sizeof(WeatherProg));//��ȡPWM��Ϣ
	timeNow = calendar.hour*60 + calendar.min; //��ȡϵͳʱ��
	
	bFindEffect = 0;
	for (i=0;i<WEATHER_SETP;i++)
	{
		if (WeatherProg[i].enable)
		{
			timeStar = WeatherProg[i].s_hour*60 + WeatherProg[i].s_minute;
			timeEnd = WeatherProg[i].e_hour*60 + WeatherProg[i].e_minute;
			if (timeNow >= timeStar && timeNow <= timeEnd) //�ڿ�ʼ�ͽ���ʱ��֮��
			{
				bFindEffect = 1;
				switch(WeatherProg[i].effect)
				{
				case WEATHER_TYPE_CLUED:  //�Ʋ�ģʽ
					if(sysInfo.workmode != MODE_CLUDE && sysInfo.workmode !=MODE_CLUDE_VEIW)
					{
						InitCloudEffect(WeatherProg[i].level);
					}
					break;

				case WEATHER_TYPE_FLASH:  //����ģʽ
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

//�������๦��
//u8 findMoonLightEffect(void)
//{
//	u16  timeNow;	//��ǰʱ��
//	u16	 timeStar, timeEnd;
//	u8 bFindEffect = 0; //�ҵ� 1��ʾ�ҵ�
//	MOONLING_s MoonlignProg;
//
//	timeNow = calendar.hour * 60 + calendar.min; //��ȡϵͳʱ��
//	STM_FLASH_READ(FLASH_MOONLIGHT_ADDR + (calendar.w_date - 1)*(sizeof(MOONLING_s)), (u16*)&MoonlignProg, sizeof(MoonlignProg));//��ȡPWM��Ϣ
//	timeStar = MoonlignProg.s_hour * 60 + MoonlignProg.s_minute;		//���࿪ʼʱ��
//	timeEnd = MoonlignProg.e_hour * 60 + MoonlignProg.e_minute;			//�������ʱ��
//	if (MoonlignProg.enable == 1)
//	{
//		if (timeEnd >= timeStar)		//��ͬһ��ʱ����
//		{
//			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
//			{
//				bFindEffect = 1;
//				InitMoonLightEffect(MoonlignProg.level);
//			}
//		}
//		else		//����ͬһ��ʱ����
//		{
//
//			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
//			{
//				bFindEffect = 1;
//				InitMoonLightEffect(MoonlignProg.level);
//			}
//		}
//	}
//	return bFindEffect;
//}
//��ʼ���Ʋ�
void InitCloudEffect(u8 level)
{
	level = 100 - level;
	if (level)SetPwmSpeed(level); //�����ٶ�
	else SetPwmSpeed(1); //Ĭ���ٶ�Ϊ2
	SetEffectFun(CloudEffect);
	SetWorkMode(MODE_CLUDE);
}

//��ʼ������
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

//��ʼ������Ч��
void InitMoonLightEffect(u8 Level)
{
	moonLight_leve = Level;
	SetPwmSpeed(PWM_SPEED_FLASH);
	SetEffectFun(MoonLingEffect);
	SetWorkMode(MODE_MOONLIGHT);
}

//ɾ����Ч
void DeleteEffect(void)
{
	SetWorkMode(MODE_TIMER);
	//EffectFunc = &NullEffect;
	SetEffectFun(NullEffect);
}

//�Ʋ�Ч��
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

//����Ч��
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

//����Ч��
void FlashEffect(void)
{
	u8 type; //��˸���� 
	u8 pwmX[3];
	if (!flash_cnt)
	{
		if (!flash_num) //������
		{
			flash_cnt = Random(20,FLASH_SPPED);
			flash_num = RanFlashNum[Random(0,RANNUM-1)]; //�������Ĵ���
		}
		else
		{
			flash_num--;
			flash_cnt = 20;
		}

		type = Random(0,FLASH_TYPE_NUM-1); //��������

		pwmX[0] =(float)(flashEffectData[type].pwm1- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM; //�㷨��(�߶ȣ��������)*����+�������
		pwmX[1] =(float)(flashEffectData[type].pwm2- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM;
		pwmX[2] =(float)(flashEffectData[type].pwm3- FLASH_MIN_PWM)* flash_scale + FLASH_MIN_PWM;

		SetPwmA(pwmX[0],
				pwmX[1],
				pwmX[2],
				FLASH_PWM,
				FLASH_PWM,
				FLASH_PWM
			);
		cntOff = Random(2,10); //�ּ�ʱ��
	}
	else
	{
		flash_cnt--;
	}

	//�ص���������
	if (!cntOff)
	{
		SetPwmA(flashOndata.pwm1,
				flashOndata.pwm2,
				flashOndata.pwm3,
				flashOndata.pwm4,
				flashOndata.pwm5,
				flashOndata.pwm6			
			); //����
	}
	else
	{
		cntOff--;
	}
}

//ȡ����� ,�˺�����һ��������
u16 Random(u16 min,u16 max)
{
	u16 temp;
	temp = SysTick->VAL;  //��TIM3�ļ�����
	temp = temp % (max+1);
	if (temp < min)
	{
		temp = min;
	}
	return temp;
}

//ȡ���Ķ�ʱ��
u8 GetMaxEffectStep(void)
{
	u8 setp;
	u8 i;

	STM_FLASH_READ(FLASH_EFFECT_ADDR,(u16*)WeatherProg,sizeof(WeatherProg));//��ȡPWM��Ϣ

	setp = 0;
	for (i=0;i<WEATHER_SETP;i++)
	{
		if (WeatherProg[i].enable) //��ʱ�������ͼ�1
		{
			setp++;
		}
		else  
		{
			break;  //����ѭ��
		}
	}

	return setp;
}

//---------end of file-------------
