/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: key.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �������� 
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
�������ֵ
R1=2.2K
R4=1K		ADC=1280
R5=2.2K		ADC=2048
R6=4.7K		ADC=2790
R7=10K		ADC=3357
*/



//������ʾ��
#define  ADC_K1_A		1460
#define  ADC_K2_A		2270
#define  ADC_K3_A		3500
#define  ADC_K4_A		3000
//#define  ADC_OFFSET		500

//��ֵ
typedef enum _key{
	KEY_OFF = 0,//û���κμ�����
	KEY_EXIT,
	KEY_ENTER,
	KEY_UP,
	KEY_DOWN,
	KEY_RESET,
}KeyValue_ENUM;

/*����״̬ö��*/
typedef enum {
	keyClear_flag   = 0x00, 
	keyDec_flag		= 0x01,
	keyAdd_flag		= 0x02,
}KeyStatue_ENUM;

#define KEY_DOWN_CNT  50  //����ʱ��
#define KEY_SPEED	6	//�ظ��ٶ�

//extern KeyStatue_ENUM	KeyStatue;  //����״̬
//extern KeyValue_ENUM	nowKeyValue; //��ǰ��ֵ

KeyValue_ENUM KeyDriver(void);
void add_dec_u8(u8 *Value,u8 ValueMax,u8 ValueMin,u8 loop);
KeyValue_ENUM TaskKeyScan(void);  //ɨ�谴��
void TouchCheck(void); //����У׼
void SavaTouchParam(void); //���津��������
void ReadTouchParam(void); //��ȡ����������
void Choose(u8 *Value,u8 numA,u8 numB);

void SetKeyStaue(KeyStatue_ENUM sta); //���ð���״̬

#endif 
/*-----------end of file-----------------*/
