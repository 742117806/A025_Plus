/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: key.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 按键驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __KEY_H
#define __KEY_H	
#include "sys.h"
//#include "include.h"
/*
电阻绝对值
R1=2.2K
R4=1K		ADC=1280
R5=2.2K		ADC=2048
R6=4.7K		ADC=2790
R7=10K		ADC=3357
*/



//配新显示屏
#define  ADC_K1_A		1460
#define  ADC_K2_A		2270
#define  ADC_K3_A		3500
#define  ADC_K4_A		3000
//#define  ADC_OFFSET		500

//键值
typedef enum _key{
	KEY_OFF = 0,//没有任何键按下
	KEY_EXIT,
	KEY_ENTER,
	KEY_UP,
	KEY_DOWN,
	KEY_RESET,
}KeyValue_ENUM;

/*按键状态枚举*/
typedef enum {
	keyClear_flag   = 0x00, 
	keyDec_flag		= 0x01,
	keyAdd_flag		= 0x02,
}KeyStatue_ENUM;

#define KEY_DOWN_CNT  50  //按下时长
#define KEY_SPEED	6	//重复速度

//extern KeyStatue_ENUM	KeyStatue;  //按键状态
//extern KeyValue_ENUM	nowKeyValue; //当前键值

KeyValue_ENUM KeyDriver(void);
void add_dec_u8(u8 *Value,u8 ValueMax,u8 ValueMin,u8 loop);
KeyValue_ENUM TaskKeyScan(void);  //扫描按键
void TouchCheck(void); //触摸校准
void SavaTouchParam(void); //保存触摸键参数
void ReadTouchParam(void); //读取触摸键参数
void Choose(u8 *Value,u8 numA,u8 numB);

void SetKeyStaue(KeyStatue_ENUM sta); //设置按键状态

#endif 
/*-----------end of file-----------------*/
