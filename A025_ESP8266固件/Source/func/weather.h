/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: weather.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 天气效果
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _WEATHER_H
#define _WEATHER_H

#include "sys.h"

#define WEATHER_TYPE_FLASH	1	//闪电
#define WEATHER_TYPE_CLUED	2	//云彩
#define WEATHER_TYPE_MOONLIGHT	3	//月相

#define WEATHER_SETP	4 //一天可以设置4组天气

//天气结构体
typedef struct {
	u8 enable;	//0:关闭；1:开启
	u8 effect;	//天气类型
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //强弱等级，从1~100
	u8 nc;	//nc 没有使用，只是为了保证字节对齐
}WEATHER;

//月相数据结构体
typedef struct 
{		
	u8 enable;	//0:关闭；1:开启
	u8 day;		//日期（1-28号）
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //强弱等级，从1~100
	u8 nc;	//nc 没有使用，只是为了保证字节对齐
}MOONLING_s;

extern WEATHER WeatherProg[WEATHER_SETP]; //天气
extern const WEATHER weather_data[WEATHER_SETP];
extern const MOONLING_s moonling_data[28];

extern void (*EffectFunc)(void); //要执行的灯具效果

#define SetEffectFun(X)		EffectFunc=X

u8 findCurrectEffect(void);
//u8 findMoonLightEffect(void);
void NullEffect(void);//空函数，无灯光特效
void CloudEffect(void); //云彩
void MoonLingEffect(void);
void FlashEffect(void); //闪电

void DeleteEffect(void); //删除灯光特效
void InitFlashEffect(u8 level); 
void InitMoonLightEffect(u8 Level);
void InitCloudEffect(u8 level);

u16 Random(u16 min,u16 max);
u8 GetMaxEffectStep(void);

#endif

//-------end of file---------
