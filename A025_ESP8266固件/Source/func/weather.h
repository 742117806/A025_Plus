/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: weather.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ����Ч��
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _WEATHER_H
#define _WEATHER_H

#include "sys.h"

#define WEATHER_TYPE_FLASH	1	//����
#define WEATHER_TYPE_CLUED	2	//�Ʋ�
#define WEATHER_TYPE_MOONLIGHT	3	//����

#define WEATHER_SETP	4 //һ���������4������

//�����ṹ��
typedef struct {
	u8 enable;	//0:�رգ�1:����
	u8 effect;	//��������
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //ǿ���ȼ�����1~100
	u8 nc;	//nc û��ʹ�ã�ֻ��Ϊ�˱�֤�ֽڶ���
}WEATHER;

//�������ݽṹ��
typedef struct 
{		
	u8 enable;	//0:�رգ�1:����
	u8 day;		//���ڣ�1-28�ţ�
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //ǿ���ȼ�����1~100
	u8 nc;	//nc û��ʹ�ã�ֻ��Ϊ�˱�֤�ֽڶ���
}MOONLING_s;

extern WEATHER WeatherProg[WEATHER_SETP]; //����
extern const WEATHER weather_data[WEATHER_SETP];
extern const MOONLING_s moonling_data[28];

extern void (*EffectFunc)(void); //Ҫִ�еĵƾ�Ч��

#define SetEffectFun(X)		EffectFunc=X

u8 findCurrectEffect(void);
//u8 findMoonLightEffect(void);
void NullEffect(void);//�պ������޵ƹ���Ч
void CloudEffect(void); //�Ʋ�
void MoonLingEffect(void);
void FlashEffect(void); //����

void DeleteEffect(void); //ɾ���ƹ���Ч
void InitFlashEffect(u8 level); 
void InitMoonLightEffect(u8 Level);
void InitCloudEffect(u8 level);

u16 Random(u16 min,u16 max);
u8 GetMaxEffectStep(void);

#endif

//-------end of file---------
