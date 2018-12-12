/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ϵͳ��������
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/
#include "func.h"
#include "menu.h"
#include "key.h"
#include "oled.h"
#include "FAN.h"
//#include "HLK_M30.h"
#include "ESP8266_07S.h"
#include "rtc.h"
#include "system.h"
#include "usart.h"
#include "stmflash.h"
#include "my_type.h"
#include "rtx_os.h"
#include "timer.h"
#include "temperature.h"
#include "flash.h"
#include "TimerFunc.h"
#include "pwmData.h"
#include "weather.h"
#include "led.h"
#include "delay.h"
#include "KEY_UPDATA.h"
#include "gbk_text.h"
#include "logo.h"
#include "display.h"
#include "string.h"

//#include "uart_pwm.h"

extern void ChangeMenu(KeyValue_ENUM key);
extern SubChoose subInfo;

_calendar_obj now;  //ʱ��
TIMER manualPwm={1,10,10,15,15,15,15,15,15}; //�ֶ�����

void (*ClockFunc)(void) = NullFucn; //ÿ��Ҫִ�еĺ�������

u8 saveInfo; //������Ϣ
u16 timeBuf; //�洢ʱ��
u8 previewTyp=0; //Ԥ������
u8 wifiInfo=0; //1:WIFI������ 0:WIFIδ����

SYSINFO sysInfo;
SYSFLAG sysFlag={0x0000};

u8 CPU_GUID[12]; //96λоƬΨһ��ʶ

MotorInfo_s motorInfo; //��������Ϣ

//uint8_t  connectingWiFiFlag = 0;	//��������WiFi·����ָʾ��
		
//Ԥ���趨�õĵ��������Ϣ����
const MotorInfo_s presetMotor = //��������Ϣ
{

	0x00,
	0x08,
	0x15,
	0x0a,
	0x12,
	0x0b,
	0x13,
	0x0c,
	0x15,
	0x10,
	0x10,
	0x11,
	0x12,
	0x03,
	0x10,
	0x05,
};


#define SAVE_TIME	0x01	//����ʱ��
#define SAVE_TIMER_SET	0x02	//���涨ʱ����
#define SAVE_MANUAL 0x04	//�ֶ�ģʽ
#define SAVE_EFFECT	0x08	//�ƹ�Ч��
#define SAVE_YES	0x10	//YES��NOѡ��
#define SAVE_PREVIEW	0x20 //��ʱԤ��
#define SAVE_LANGUAGE	0x40 //��������

//�պ���
void NullFucn(void){}

//��ȡEasy Timer��ֵ
void ReadEasyTimer()
{
	if(!(saveInfo & SAVE_TIMER_SET))  //�Ӳ˵����룬Ҫ��ȡ����
	{
		saveInfo |= SAVE_TIMER_SET;
		STM_FLASH_READ(FLASH_EASY_TIMER,(u16*)&esayTimer,sizeof(esayTimer)); //��ȡEASY ��ʱ��Ϣ
	}
}

//��ʾ3·��6·PWM����
void ShowPwmUI(u8 p1,u8 p2,u8 p3,u8 p4,u8 p5,u8 p6,u8 index,u8 page)
{
	OLED_Clear_Ram();
	if (sysInfo.pwmNumber == 3) //3·
	{
		OLED_ShowString(0,16,"   %     %     %");
		//��ʾ����

		IF_EN()ShowStringCN(0,0,"  A     B     C ");
		IF_CN()ShowStringCN(0,0," A·   B·   C·");
		OLED_ShowNum( 0,16,p1,3,16);
		OLED_ShowNum(48,16,p2,3,16);
		OLED_ShowNum(96,16,p3,3,16);

		switch(index)
		{
		case 0:
			break;
		case 1:
			IF_EN() OLED_ShowChar(16,0,'A',16,0);
			IF_CN() OLED_FLIP(8,0,24);
			break;
		case 2:
			IF_EN() OLED_ShowChar(64,0,'B',16,0);
			IF_CN() OLED_FLIP(56,0,24);
			break;
		case 3:
			IF_EN() OLED_ShowChar(112,0,'C',16,0);
			IF_CN() OLED_FLIP(104,0,24);
			break;
		}
	}
	else  //--------------------6·
	{
		OLED_ShowString(0,16,"   %     %     %");

		//��ʾ����
		switch(page)
		{
		case 0:
			IF_EN()ShowStringCN(0,0,"  A     B     C ");
			IF_CN()ShowStringCN(0,0," A·   B·   C·");
			OLED_ShowNum( 0,16,p1,3,16);
			OLED_ShowNum(48,16,p2,3,16);
			OLED_ShowNum(96,16,p3,3,16);
			break;

		case 1:
			IF_EN()ShowStringCN(0,0,"  D     E     F ");
			IF_CN()ShowStringCN(0,0," D·   E·   F·");
			OLED_ShowNum( 0,16,p4,3,16);
			OLED_ShowNum(48,16,p5,3,16);
			OLED_ShowNum(96,16,p6,3,16);
			break;
		}

		switch(index)
		{
		case 0:
			break;
		case 1:
			OLED_FLIP(0,1,32);
			break;
		case 2:
			OLED_FLIP(48,1,32);
			break;
		case 3:
			OLED_FLIP(96,1,32);
			break;
		case 4:
			OLED_FLIP(0,1,32);
			break;
		case 5:
			OLED_FLIP(48,1,32);
			break;
		case 6:
			OLED_FLIP(96,1,32);
			break;
		}
	} //6·

	OS_UPDATA_OLED();
}

//�ճ�����
void SunriseSet(void)
{
	subInfo.maxSetp = 1;

	ReadEasyTimer();

	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Set Sunrise time");
	IF_CN() ShowStringCN(0,0," �����ճ�ʱ��");

	ShowString(5,1,"10:30");
	ShowTwoNum(5,1,esayTimer.s_hour,1);
	ShowTwoNum(8,1,esayTimer.s_minute,1);

	switch(subInfo.step)
	{
	case 0:  //ʱ
		add_dec_u8(&esayTimer.s_hour,12,1,TRUE);   //�޶��ճ�ʱ�䷶Χ��1~12��
		ShowTwoNum(5,1,esayTimer.s_hour,0);
		break;
	case 1:  //��
		add_dec_u8((u8*)&esayTimer.s_minute,59,0,TRUE); 
		ShowTwoNum(8,1,esayTimer.s_minute,0);		
		break;
	}
	OS_UPDATA_OLED();
}

//��������
void SundownSet(void)
{
	subInfo.maxSetp = 1;

	ReadEasyTimer();
	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Set Sunset time");
	IF_CN() ShowStringCN(0,0," ��������ʱ��");

	ShowString(5,1,"10:30");
	ShowTwoNum(5,1,esayTimer.e_hour,1);
	ShowTwoNum(8,1,esayTimer.e_minute,1);

	switch(subInfo.step)
	{
	case 0:  //ʱ
		add_dec_u8(&esayTimer.e_hour,23,13,TRUE); 
		ShowTwoNum(5,1,esayTimer.e_hour,0);
		break;
	case 1:  //��
		add_dec_u8((u8*)&esayTimer.e_minute,59,0,TRUE); 
		ShowTwoNum(8,1,esayTimer.e_minute,0);		
		break;
	}
	OS_UPDATA_OLED();
}

//��������
void SunlightSet(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp  = 6;
	ReadEasyTimer();

	//
	if (sysInfo.pwmNumber == 3) //3·
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmDay.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
		}
		else	
		{
			SetKeyStaue(keyClear_flag); //3·���治�ܷ�ҳ
		}
		
		esayTimer.pwmDay.pwm4 = 0;
		esayTimer.pwmDay.pwm5 = 0;
		esayTimer.pwmDay.pwm6 = 0;
		page = 0;
	}
	else  //--------------------6·
	{
		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmDay.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
			if (subInfo.step <= 3) page = 0;
			else page = 1;
		}
		else	
		{
			add_dec_u8(&page,1,0,TRUE);  //6·,�����ֶ�ģʽ�������¼��ɷ�ҳ	
		}
	}

	ShowPwmUI(
		esayTimer.pwmDay.pwm1,
		esayTimer.pwmDay.pwm2,
		esayTimer.pwmDay.pwm3,
		esayTimer.pwmDay.pwm4,
		esayTimer.pwmDay.pwm5,
		esayTimer.pwmDay.pwm6,
		subInfo.step,
		page
		);
}

//�¹���������
void MoonlightSet(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp  = 6;
	ReadEasyTimer();

	if (sysInfo.pwmNumber == 3) //3·
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmMoon.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
		}
		else	
		{
			SetKeyStaue(keyClear_flag); //3·���治�ܷ�ҳ
		}

		page = 0;

		//����3·��Ϊ0
		esayTimer.pwmMoon.pwm4 = 0;
		esayTimer.pwmMoon.pwm5 = 0;
		esayTimer.pwmMoon.pwm6 = 0;

	}
	else  //--------------------6·
	{
		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmMoon.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
			if (subInfo.step <= 3) page = 0;
			else page = 1;
		}
		else	
		{
			add_dec_u8(&page,1,0,TRUE);  //6·,�����ֶ�ģʽ�������¼��ɷ�ҳ	
		}
	} //6·

	ShowPwmUI(
		esayTimer.pwmMoon.pwm1,
		esayTimer.pwmMoon.pwm2,
		esayTimer.pwmMoon.pwm3,
		esayTimer.pwmMoon.pwm4,
		esayTimer.pwmMoon.pwm5,
		esayTimer.pwmMoon.pwm6,
		subInfo.step,
		page
		);
}

//�����ֶ���ʱ����
void SaveEasyTimer(void)
{
	u8 i;
	u16 t; //ʱ��
	u8 hour,munute;
	for (i=0;i<=PRGM_STEP;i++) //�ȹص����еĶ�ʱ
	{
		TimerProg[i].enable = 0;
	}

	//�ճ�,�ճ�ʱ�����������¹������
	TimerProg[0].enable = 1;
	TimerProg[0].hour = esayTimer.s_hour;
	TimerProg[0].minute = esayTimer.s_minute;
	TimerProg[0].pwm1 = esayTimer.pwmMoon.pwm1;
	TimerProg[0].pwm2 = esayTimer.pwmMoon.pwm2;
	TimerProg[0].pwm3 = esayTimer.pwmMoon.pwm3;
	TimerProg[0].pwm4 = esayTimer.pwmMoon.pwm4;
	TimerProg[0].pwm5 = esayTimer.pwmMoon.pwm5;
	TimerProg[0].pwm6 = esayTimer.pwmMoon.pwm6;

	//�ճ�ʱ��
	t = esayTimer.s_hour*60 + esayTimer.s_minute + SUNRISE_TIME;
	hour = t / 60;
	munute = t % 60;
	TimerProg[1].enable = 1;
	TimerProg[1].hour = hour;
	TimerProg[1].minute = munute;
	TimerProg[1].pwm1 = esayTimer.pwmDay.pwm1;
	TimerProg[1].pwm2 = esayTimer.pwmDay.pwm2;
	TimerProg[1].pwm3 = esayTimer.pwmDay.pwm3;
	TimerProg[1].pwm4 = esayTimer.pwmDay.pwm4;
	TimerProg[1].pwm5 = esayTimer.pwmDay.pwm5;
	TimerProg[1].pwm6 = esayTimer.pwmDay.pwm6;

	//����
	t = esayTimer.e_hour*60 + esayTimer.e_minute - SUNRISE_TIME;
	hour = t / 60;
	munute = t % 60;
	TimerProg[2].enable = 1;
	TimerProg[2].hour = hour;
	TimerProg[2].minute = munute;
	TimerProg[2].pwm1 = esayTimer.pwmDay.pwm1;
	TimerProg[2].pwm2 = esayTimer.pwmDay.pwm2;
	TimerProg[2].pwm3 = esayTimer.pwmDay.pwm3;
	TimerProg[2].pwm4 = esayTimer.pwmDay.pwm4;
	TimerProg[2].pwm5 = esayTimer.pwmDay.pwm5;
	TimerProg[2].pwm6 = esayTimer.pwmDay.pwm6;

	//�¹�
	TimerProg[3].enable = 1;
	TimerProg[3].hour = esayTimer.e_hour;
	TimerProg[3].minute = esayTimer.e_minute;
	TimerProg[3].pwm1 = esayTimer.pwmMoon.pwm1;
	TimerProg[3].pwm2 = esayTimer.pwmMoon.pwm2;
	TimerProg[3].pwm3 = esayTimer.pwmMoon.pwm3;
	TimerProg[3].pwm4 = esayTimer.pwmMoon.pwm4;
	TimerProg[3].pwm5 = esayTimer.pwmMoon.pwm5;
	TimerProg[3].pwm6 = esayTimer.pwmMoon.pwm6;

	STM_FLASH_WRITE(FLASH_TIMER_ADDR,(u16*)&TimerProg,sizeof(TimerProg)); //���涨ʱ��Ϣ
	STM_FLASH_WRITE(FLASH_EASY_TIMER,(u16*)&esayTimer,sizeof(esayTimer)); //����EASY ��ʱ��Ϣ
	//�����ֶ���ʱ��Ϣ
}

/*-----------------------------------
��  �����ֶ�ģʽ
��  �ܣ�
��  �룺
��  ����
-----------------------------------*/
void ManualSetMenu(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp = 7;

	SetWorkMode(MODE_MUNUAL); //PWM��Ϊ�ֶ�ģʽ

	if (!(saveInfo & SAVE_MANUAL))  //�Ӳ˵����룬Ҫ��ȡ����
	{
		saveInfo |= SAVE_MANUAL;
		STM_FLASH_READ(FLASH_MANUAL_PWM, (u16*)&manualPwm, sizeof(manualPwm)); //��ȡ�ֶ���PWM
	}


	if (sysInfo.pwmNumber == 3) //3·
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step)
		{
			pPwm = &manualPwm.pwm1 + (subInfo.step - 1);
			add_dec_u8(pPwm, 100, 0, TRUE);
		}
		else
		{
			SetKeyStaue(keyClear_flag); //3·���治�ܷ�ҳ
		}

		page = 0;

		//����3·��Ϊ0
		manualPwm.pwm4 = 0;
		manualPwm.pwm5 = 0;
		manualPwm.pwm6 = 0;

	}
	else  //--------------------6·
	{
		if (subInfo.step)
		{
			pPwm = &manualPwm.pwm1 + (subInfo.step - 1);
			add_dec_u8(pPwm, 100, 0, TRUE);
			if (subInfo.step <= 3) page = 0;
			else page = 1;
		}
		else
		{
			add_dec_u8(&page, 1, 0, TRUE);  //6·,�����ֶ�ģʽ�������¼��ɷ�ҳ	
		}
	} //6·

	SetPwm(
		manualPwm.pwm1,
		manualPwm.pwm2,
		manualPwm.pwm3,
		manualPwm.pwm4,
		manualPwm.pwm5,
		manualPwm.pwm6
		);
	if (subInfo.step == 7)
	{
		page = 0;
		ChangeMenu(KEY_EXIT);
	}
	else
	{
		ShowPwmUI(
			manualPwm.pwm1,
			manualPwm.pwm2,
			manualPwm.pwm3,
			manualPwm.pwm4,
			manualPwm.pwm5,
			manualPwm.pwm6,
			subInfo.step,
			page
			);
	}
}

/*-----------------------------------
��  ������ʾ��ǰPWM����
��  �ܣ�
��  �룺
��  ����
-----------------------------------*/
void ShowCurrentPwmMenu(void)
{
	u8 temp;
	SetClockFun(ShowCurrentPwmMenu); //��ʱ��ʾ
	OLED_Clear_Ram();

	//��ʾ����
	if (sysInfo.pwmNumber == 3) //3·
	{
		IF_EN()ShowStringCN(0,0,"  A     B     C ");
		IF_CN()ShowStringCN(0,0," A·   B·   C·");
		OLED_ShowString(0,16,"   %     %     %");
		OLED_ShowNum( 0,16,GetPwm(1),3,16);
		OLED_ShowNum(48,16,GetPwm(2),3,16);
		OLED_ShowNum(96,16,GetPwm(3),3,16);
	}
	else  //6·
	{
		OLED_ShowString(0,0, "A:");
		OLED_ShowString(42,0,"B:");
		OLED_ShowString(88,0,"C:");
		OLED_ShowString(0,16, "D:");
		OLED_ShowString(42,16,"E:");
		OLED_ShowString(88,16,"F:");

		temp = GetPwm(1);
		if (temp > 99)OLED_ShowNum(14,0,temp,3,16); //3λ
		else OLED_ShowNum(14,0,temp,2,16);

		temp = GetPwm(2);
		if (temp > 99)OLED_ShowNum(58,0,temp,3,16); //3λ
		else OLED_ShowNum(58,0,temp,2,16);

		temp = GetPwm(3);
		if (temp > 99)OLED_ShowNum(102,0,temp,3,16); //3λ
		else OLED_ShowNum(102,0,temp,2,16);

		temp = GetPwm(4);
		if (temp > 99)OLED_ShowNum(14,16,temp,3,16); //3λ
		else OLED_ShowNum(14,16,temp,2,16);

		temp = GetPwm(5);
		if (temp > 99)OLED_ShowNum(58,16,temp,3,16); //3λ
		else OLED_ShowNum(58,16,temp,2,16);

		temp = GetPwm(6);
		if (temp > 99)OLED_ShowNum(102,16,temp,3,16); //3λ
		else OLED_ShowNum(102,16,temp,2,16);

	}

	OS_UPDATA_OLED();
}

//����
void Help(void)
{
	ShowString(0,0,"      Help    ");
	ShowString(0,1,"==============");
	OS_UPDATA_OLED();
}

//����ʱ������
void SetDataTime(void)
{
	static u8 year;

	subInfo.maxSetp = 5;

	if(!(saveInfo & SAVE_TIME)) //�Ӳ˵����룬���¶�ȡʱ��
	{
		saveInfo |= SAVE_TIME; //���
		//��ȡ��ǰʱ��
		now.hour = calendar.hour;
		now.min = calendar.min;
		now.sec = calendar.sec;
		now.w_date = calendar.w_date;
		now.w_month = calendar.w_month;
		year = calendar.w_year-2000; //�����u16��Ҫת��8λ�ģ�2015-2000 =15
		now.week = calendar.week;
	}

	OLED_Clear_Ram();
	ShowString(0,0,"2015-00-00");
	ShowString(0,1,"10:30:59");
	
	ShowTwoNum(2,0,year,1);
	ShowTwoNum(5,0,now.w_month,1);
	ShowTwoNum(8,0,now.w_date,1);
	ShowTwoNum(0,1,now.hour,1);
	ShowTwoNum(3,1,now.min,1);
	ShowTwoNum(6,1,now.sec,1);

	switch(subInfo.step)
	{
	case 0:  //��
		add_dec_u8(&year,99,0,TRUE); 
		ShowTwoNum(2,0,year,0);
		break;
	case 1:  //��
		add_dec_u8((u8*)&now.w_month,12,0,TRUE); 
		ShowTwoNum(5,0,now.w_month,0);		
		break;
	case 2:  //��
		add_dec_u8((u8*)&now.w_date,31,0,TRUE);  //����û�����ִ�С�º�����
		ShowTwoNum(8,0,now.w_date,0);
		break;
	case 3: //Сʱ
		add_dec_u8((u8*)&now.hour,23,0,TRUE);
		ShowTwoNum(0,1,now.hour,0);
		break;
	case 4: //����
		add_dec_u8((u8*)&now.min,59,0,TRUE);
		ShowTwoNum(3,1,now.min,0);
		break;
	case 5: //��
		add_dec_u8((u8*)&now.sec,59,0,TRUE);
		ShowTwoNum(6,1,now.sec,0);
		break;
	}
	now.w_year = 2000+year; //������ת����

	OS_UPDATA_OLED();
}

//�ָ�Ĭ��
void LoadDefault(void)
{
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"Load default ?  ");
			ShowStringCN(0,1,"    NO  YES     ");
		}

		IF_CN()
		{
			ShowStringCN(0,0,"�ָ�Ĭ������?");
			ShowStringCN(0,1,"    ��  ��   ");
		}
		
		if (saveInfo == SAVE_YES)
		{
			IF_EN() OLED_ShowStringA(64,16,"YES",0);
			IF_CN() OLED_FLIP(64,1,16);
		}
		else 
		{
			IF_EN() OLED_ShowStringA(32,16,"NO",0);
			IF_CN() OLED_FLIP(32,1,16);
		}
		break;
	case 1: //������OK��
		if (saveInfo == SAVE_YES) //ѡ�����YES
		{
			SystemLoadDefault(); //�ָ�Ĭ��
			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Press ESC key  ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"�ָ��ɹ���");
				ShowStringCN(0,1,"�밴ESC������");
			}
			
			saveInfo &= SAVE_YES;
		}
		else
		{
			IF_EN()
			{
				ShowStringCN(0,0,"Press ESC key  ");
				ShowStringCN(0,1,"to return ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"δ�ָ���");
				ShowStringCN(0,1,"�밴ESC������");
			}
			
		}
		subInfo.step = 0;  //���
		break;
	}
	OS_UPDATA_OLED();
}

//����ϵͳĬ��ֵ
void SystemLoadDefault(void)
{
	STM_FLASH_WRITE(FLASH_TIMER_ADDR,(u16*)&presetTimer,sizeof(presetTimer)); //���涨ʱ��Ϣ
	STM_FLASH_WRITE(FLASH_MANUAL_PWM,(u16*)&manualPwm,sizeof(manualPwm)); //�����ֶ�PWM��Ϣ
	STM_FLASH_WRITE(FLASH_EASY_TIMER,(u16*)&presetEsayTimer,sizeof(presetEsayTimer)); //����EASY ��ʱ��Ϣ
	STM_FLASH_WRITE(FLASH_EFFECT_ADDR,(u16*)&weather_data,sizeof(weather_data)); //����������Ϣ	
	STM_FLASH_WRITE(FLASH_MOTOR_INFO_ADDR, (u16*)&presetMotor, sizeof(presetMotor));	//����������
	//STM_FLASH_WRITE(FLASH_MOONLIGHT_ADDR, (u16*)&moonling_data, sizeof(moonling_data));	//��������Ĭ������
	//STM_FLASH_WRITE(FLASH_LANGUAGE_ADDR,(u16*)&sysInfo.language,sizeof(sysInfo.language)); //��������
	// STM_FLASH_WRITE(FLASH_INFO_ADDR,(u16*)&Sysflag,sizeof(Sysflag)); //ϵͳ��Ϣ
}


//�汾
void Version(void)
{
	u16 temp;
	OLED_Clear_Ram();
//	IF_EN()	ShowStringCN(0,0,"Software version");
//	IF_CN() ShowStringCN(0,0,"  ����汾��Ϣ  ");

	ShowStringCN(0,0," ver:xx-xx      ");
	ShowStringCN(0,1,"code:xx-xx      ");

	ShowTwoNum(5,0,version[0],1);
	ShowTwoNum(8,0,version[1],1);

	temp = GetCustomCode();

	ShowTwoNum(5,1,temp>>8,1);
	ShowTwoNum(8,1,(u8)temp,1);
	OS_UPDATA_OLED();
}

//����Ԥ����ʱ��
void SetPreveiwTime(u8 hour,u8 minute)
{
	timeBuf = hour*60+minute;
	previewTyp = 1;
}

//��ʱԤ��
void PreviewTimer(void)
{
#define DAY_MINUTE	1439  //24*60 һ��ķ�����
	u8 hour,minute;
	static u8 cnt100ms;

	SetWorkMode(MODE_PREVIEW);
	if (++cnt100ms >5)cnt100ms = 0; //50msһ��
	else return;

//	SetClockFun(PreviewTimer); //ÿ�붼ִ��
	saveInfo |= SAVE_PREVIEW;

	if (timeBuf >= DAY_MINUTE)  //�����ֹͣ
	{
		if (previewTyp) //WIFI�����µ�Ԥ��,Ҫ�Զ��ص���ʱģʽ
		{
		//	timeBuf = 0;
			previewTyp = 0;
			SetWorkMode(MODE_TIMER);
		}
	//	timeBuf = DAY_MINUTE;
	}
	else
	{
		timeBuf += 1; //1�����൱����5����
		hour = timeBuf / 60;
		minute = timeBuf % 60;

		SetPwmSpeed(PWM_SPEED_PREVIEW);
		SetPwmByTimer(hour,minute);
	}

	OLED_Clear_Ram();
	IF_EN()	ShowStringCN(0,0," Timer preview");
	IF_CN() ShowStringCN(0,0,"    ��ʱԤ��");
	ShowString(0,1,"10:00:00[00->01]");
	ShowTwoNum(0,1,hour,1);
	ShowTwoNum(3,1,minute,1);
	ShowTwoNum(9,1,sysInfo.indexTimer+1,1); //��ʾʱ���1 
	ShowTwoNum(13,1,sysInfo.nextTimer+1,1); //��ʾʱ���2
	
	OS_UPDATA_OLED();
}

//���ö�ʱԤ��
void ResetPreviewTimer(void)
{
	timeBuf = 0;
	previewTyp=0;
}

//����ʱ��
void Desktop(void)
{
	OLED_Clear_Ram();

	ShowString(4,0,"10:20:59");
	ShowTwoNum(4,0,calendar.hour,1);
	ShowTwoNum(7,0,calendar.min,1);
	ShowTwoNum(10,0,calendar.sec,1);
	if (wifiConnetFlag == 1)
	{
		//OLED_DrawImg16X16(0, 0, wifi_Image);
		
	}
	else
	{
		//OLED_DrawImg16X16(0, 0, wifi_Image_C);
	}
	
	switch(sysInfo.workmode)
	{
	case MODE_TIMER:
		IF_EN() ShowString(0,1,  " Timer[00->01]");
		IF_CN() ShowStringCN(0,1,"  ��ʱ[00->01]");
		ShowTwoNum(7,1,sysInfo.indexTimer+1,1); //��ʾʱ���1 
		ShowTwoNum(11,1,sysInfo.nextTimer+1,1); //��ʾʱ���2
		break;

	case MODE_MUNUAL:
		IF_EN() ShowString(3,1,  "[ Dimmer ]");
		IF_CN() ShowStringCN(3,1,"[  �ֶ�  ]");
		break;

	case MODE_CLUDE:
		IF_EN() ShowString(3,1,  "[  Cloud ]");
		IF_CN() ShowStringCN(3,1,"[  �Ʋ�  ]");
		break;
	case MODE_FLASH: //�ƹ���Ч
		IF_EN() ShowString(3,1,  "[  Flash ]");
		IF_CN() ShowStringCN(3,1,"[  ����  ]");
		break;
	case MODE_CLUDE_VEIW:
		IF_EN() ShowString(3,1,  "[  Cloud ]");
		IF_CN() ShowStringCN(3,1,"[�Ʋ�Ԥ��]");
		break;
	case MODE_FLASH_VEIW: //�ƹ���Ч
		IF_EN() ShowString(3,1,  "[  flash ]");
		IF_CN() ShowStringCN(3,1,"[����Ԥ��]");
		break;
	case MODE_MOONLIGHT:
		IF_EN() ShowString(3, 1, "[  Moon  ]");
		IF_CN() ShowStringCN(3, 1,"[  ����  ]");
		break;
	case MODE_MOONLIGHT_VEIW:
		IF_EN() ShowString(3, 1, "[  Moon  ]");
		IF_CN() ShowStringCN(3, 1, "[����Ԥ��]");
		break;
	case MODE_POWER_OFF:
		IF_EN() ShowString(2,1,  "[Power off]");
		IF_CN() ShowStringCN(3,1,"[  �ص�  ]");
		break;
	}

//	OS_UPDATA_OLED();
}

//��ʾ�¶�
void TempFan(void)
{
	u8 temp;
	u8 speed;
	SetClockFun(TempFan); //��ʱ��ʾ

	temp = GetTemperature()/10; 
	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Temperature:00 C");
	IF_CN() ShowStringCN(0,0,"   ��ǰ�¶�:00 C");
	if (temp > 99)ShowString(12,0,"??"); //����99�� ��ʾ??
	else ShowTwoNum(12,0,temp,1);

	speed = GetFanSpeed();
	IF_EN() ShowStringCN(0,1,"Fan speed  :xx %");
	IF_CN() ShowStringCN(0,1,"   ����ת��:xx %");
	if (speed == 100)ShowString(12,1,"100"); //��ʾ100%
	else ShowTwoNum(12,1,speed,1);

	OS_UPDATA_OLED();
}

//Wifi����ΪAPģʽ
#if 0
void WifiApModeConfig(void)
{
	//��OK��ȷ�ϣ�saveinfo��������ѡ�����YES����NO
	subInfo.maxSetp = 1; 
	
	OLED_Clear_Ram();
	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"Set to AP mode ?");
			ShowStringCN(0,1,"    NO  YES     ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"���ó��ȵ�ģʽ?");
			ShowStringCN(0,1,"    ��  ��     ");
		}
		
		if (saveInfo == SAVE_YES)
		{
			IF_EN()	OLED_ShowStringA(64,16,"YES",0);
			IF_CN() OLED_FLIP(64,1,16);
		}
		else
		{
			IF_EN() OLED_ShowStringA(32,16,"NO",0);
			IF_CN() OLED_FLIP(32,1,16);
		}
		break;
	case 1: //������OK��
		if (saveInfo == SAVE_YES) //ѡ�����YES
		{
			sysFlag.Bits.LED_sta = 1; //����ָʾ��
			sysFlag.Bits.LED_SAVE = 1;

			IF_EN()
			{
				ShowStringCN(0,0,"Config Wifi   ");
				ShowStringCN(0,1,"Waiting...    ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"��������Wifi");
				ShowStringCN(0,1,"���Ե�...  ");
			}
			
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_SetAPmode("","");
			SPI_Flash_Write((u8*)&M30_Buf,W25QXX_MAC,6);//����MAC��ַ��MAC��ַֻ��6λ
			SetUartRxType(USART_TYPE_LED);

			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Reset...     ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"���óɹ���");
				ShowStringCN(0,1,"��������ϵͳ...");
			}
			
			OS_UPDATA_OLED();
			delay_ms(2000);
			SoftReset(); //����
		}
		else
		{
			IF_EN()
			{
				ShowStringCN(0,0,"Press ESC key  ");
				ShowStringCN(0,1,"to return ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"�밴ESC������");
			}			
		}
		break;
	}
	OS_UPDATA_OLED();

}
#else
void WifiApModeConfig(void)
{
	//��OK��ȷ�ϣ�saveinfo��������ѡ�����YES����NO
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();
	if (subInfo.step == 0)
	{
		IF_EN()
		{
			OLED_ShowString(0, 8, "Run in Wifi mode");
			sysFlag.Bits.LED_sta = 1; //����ָʾ��
			sysFlag.Bits.LED_SAVE = 1;
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_SetAPmode("", "");
			SPI_Flash_Write((u8*)&M30_Buf, W25QXX_MAC, 6);//����MAC��ַ��MAC��ַֻ��6λ
			SetUartRxType(USART_TYPE_LED);
			OS_UPDATA_OLED();
		}
	}
	else
	{
		ChangeMenu(KEY_EXIT);
	}
}
#endif
//Wifi����ΪSTAģʽ
void WifiStaModeConfig(void)
{
	subInfo.maxSetp = 1;

	OLED_Clear_Ram();
	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowString(0,0,"Set to STA mode?");
			ShowString(0,1,"    NO  YES     ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"���ó�����ģʽ?");
			ShowStringCN(0,1,"    ��  ��     ");
		}

		if (saveInfo == SAVE_YES)
		{
			IF_EN()	OLED_ShowStringA(64,16,"YES",0);
			IF_CN() OLED_FLIP(64,1,16);
		}
		else
		{
			IF_EN() OLED_ShowStringA(32,16,"NO",0);
			IF_CN() OLED_FLIP(32,1,16);
		}
		break;
	case 1: //������OK��
		if (saveInfo == SAVE_YES) //ѡ�����YES
		{
			sysFlag.Bits.LED_sta = 1; //����ָʾ��
			sysFlag.Bits.LED_SAVE = 1;

			IF_EN()
			{
				ShowStringCN(0,0,"Config Wifi   ");
				ShowStringCN(0,1,"Waiting...    ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"��������Wifi");
				ShowStringCN(0,1,"���Ե�...  ");
			}
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_LoadDefault(); //�ָ�WIFIĬ��
			SetUartRxType(USART_TYPE_LED);
			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Reset...     ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"���óɹ���");
				ShowStringCN(0,1,"��������ϵͳ...");
			}
			OS_UPDATA_OLED();
			delay_ms(2000);
			SoftReset(); //����
		}
		else
		{
			IF_EN()
			{
				ShowStringCN(0,0,"Press ESC key  ");
				ShowStringCN(0,1,"to return ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"�밴ESC������");
			}
		}
		break;
	}
	OS_UPDATA_OLED();
}

//����STA��������
void ConfigWifiStaConn(void)
{
	SetUartRxType(USART_TYPE_AT);
	conTowifi();
	SetUartRxType(USART_TYPE_LED);
}

//WIFI��Ϣ
void WifiInfo(void)
{
	u8 temp_IP[17]={0};
	OLED_Clear_Ram();
	//SetClockFun(WifiInfo);		//ȡ��ÿ�붼ȥ��ʾWIFI��Ϣ������Ӱ�찴����������
	SetUartRxType(USART_TYPE_AT);

	sysFlag.Bits.LED_sta = 1; //����ָʾ��

	IF_EN()
	{
		ShowStringCN(0,0,"Info Reading    ");
		ShowStringCN(0,1,"Please Wait.....");
	}

	IF_CN()

	{
		ShowStringCN(0,0,"�鿴��          ");
		ShowStringCN(0,1,"���Ե�......    ");
	}

	OS_UPDATA_OLED();
	//if(!HLK_M30_EnterATmode()) //�������ATģʽҪ��
	{
		OLED_Clear_Ram();
		ESP8266_GetIp();
		memcpy(temp_IP,M30_Buf,sizeof(temp_IP));
		if (temp_IP[0] == '0') //û��ȡ��IP
		{
		//	wifiInfo &= ~0x01;
			SetLedIndicator(LED_ITR_TWO);
			IF_EN()
			{
				ShowStringCN(0,0,"Not Connection ");
				ShowStringCN(0,0,"wait for conn..");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"Wifiδ����");
				ShowStringCN(0,1,"�ȴ�������...");
			}
		}
		else
		{
			wifiInfo |= 0x01;
			
			ShowString(0, 0, ESP8266_GetWifiName());
			ShowString(0, 1, (const u8*)temp_IP);
			
			SetLedIndicator(LED_ITR_ON);
		}
	}
//	else
//	{
//		IF_EN()	ShowStringCN(0,0,"Read error!");
//		IF_CN() ShowStringCN(0,0,"Wifi��ȡʧ��!");
//	}
	//HLK_M30_ExitATmode(); //�˳�ATģʽ
	SetUartRxType(USART_TYPE_LED);

	if (wifiInfo == 0x81) //�ɹ���ȡIP���˳�
	{
		SetClockFun(Desktop);
		wifiInfo &= ~0x80;
		SetBeepType(BEEP_THREE);//��ȡ��IP��������
	}
	OS_UPDATA_OLED();
}

//�˳��Ӳ������淵�ز˵�Ҫ�������
void ReturnToMenu(void)
{
	if (saveInfo & SAVE_TIME) //����ʱ��
	{
		saveInfo &= ~SAVE_TIME;
		RTC_Set(
			now.w_year,
			now.w_month,
			now.w_date,
			now.hour,
			now.min,
			now.sec
			); //����ʱ��
	}
	else if (saveInfo & SAVE_MANUAL) //�����ֶ�PWM
	{
		saveInfo &= ~SAVE_MANUAL;
		STM_FLASH_WRITE(FLASH_MANUAL_PWM,(u16*)&manualPwm,sizeof(manualPwm)); //����PWM��Ϣ
		//workmode &= ~MODE_MUNUAL;
		SetWorkMode(MODE_TIMER);
	}
	else if (saveInfo & SAVE_TIMER_SET)//���涨ʱ
	{
		saveInfo &= ~SAVE_TIMER_SET;
		SaveEasyTimer();
	}
	else if (saveInfo & SAVE_EFFECT)//�˳�Ч��ģʽ
	{
		saveInfo &= ~SAVE_EFFECT;
		SetWorkMode(MODE_TIMER);
	}
	else if (saveInfo & SAVE_YES) //YES��NO
	{
		saveInfo &= ~SAVE_YES;
	}
	else if (saveInfo &SAVE_PREVIEW)
	{
		saveInfo &= ~SAVE_PREVIEW;
		SetWorkMode(MODE_TIMER);
		timeBuf = 0;
	}
	else if (saveInfo & SAVE_LANGUAGE) //����
	{
		saveInfo &= ~SAVE_LANGUAGE;
		saveLanguage();
	}

	SetClockFun(NullFucn); //�������ʾ

	SetLedIndicator(LED_ITR_ON); //ָʾ�Ƴ���
}

//��ʾ0~99����λ��
void ShowTwoNum(u8 x,u8 y,u8 num,u8 mode)
{
	if (num <10)
	{
		OLED_ShowChar(x*8,y*16,'0',FONT16,mode); 
		OLED_ShowChar(x*8+8,y*16,'0'+num,FONT16,mode); 
	}
	else
	{
		OLED_ShowChar(x*8,y*16,'0'+num/10,FONT16,mode); 
		OLED_ShowChar(x*8+8,y*16,'0'+num%10,FONT16,mode); 
	}
} 


//���ÿͻ�����
void SetCustomCode(u16 code1)
{	
	SPI_Flash_Write((u8*)&code1,W25QXX_CUSTOM_CODE,sizeof(code1));//д��ͻ���Ǵ���
}

//��ȡ�ͻ�����
u16 GetCustomCode(void)
{	
	u16 code1;
	SPI_Flash_Read((u8*)&code1,W25QXX_CUSTOM_CODE,sizeof(code1));
	return code1;
}

//���õ���·��
void SetPwmNumber(u8 num)
{
	SPI_Flash_Write((u8*)&num,W25QXX_PWM_NUM,sizeof(num));
}

//��ȡ����·��
u8 GetPwmNumber(void)
{
	u8 num;
	SPI_Flash_Read((u8*)&num,W25QXX_PWM_NUM,sizeof(num));	
	if (num > 6) //
	{
		num = 6;
	}
	return num;
}


//��ȡGUID��index = 0~11
u8 GetGUID(u8 index)  //
{
	/*  CPU ID
	u8 id;
	if (index <= 12)
	{
		id = CPU_GUID[index];
	}
	else
	{
		id = 0;
	}
	return id;
	*/

	u8 id;
	if (index < 6)
	{
		id = CPU_GUID[index];
	}
	else
	{
		id = 0;
	}
	return id;
}

//��ȡGUID��������
void ReadGUID(void)
{
	/*  ��CPU��GUID
	u8 i;
	u8 *p;
	u8 temp;
	p = CPU_GUID;
	for(i=0;i<12;i++)
	{
		temp =(u8)*(vu32*)(0x1FFFF7E8+i);
		if(temp > 0x80)temp = 0xFF-temp; //ȷ��CPUIDС��0x80,�����������ͻ
		*p++ = temp;
	}
	*/

	//��MAC
	//SPI_Flash_Read((u8*)&CPU_GUID,W25QXX_MAC,6);
}

//�ۿ�����Ч��
void ViewFlashEffect(void)
{
	if(!(saveInfo & SAVE_EFFECT))  //�Ӳ˵����룬Ҫ��ȡ����
	{
		saveInfo |= SAVE_EFFECT;
	}
	OLED_Clear_Ram();
	
	IF_EN() ShowStringCN(0,0,"Lightning...");
	IF_CN() ShowStringCN(0,0,"������...");

	InitFlashEffect(20);
	SetWorkMode(MODE_FLASH_VEIW);
	OS_UPDATA_OLED();
}

//�ۿ��Ʋ�Ч��
void ViewCloudEffect(void)
{
	if(!(saveInfo & SAVE_EFFECT))  //�Ӳ˵����룬Ҫ��ȡ����
	{
		saveInfo |= SAVE_EFFECT;
	}
	OLED_Clear_Ram();

	IF_EN()	ShowStringCN(0,0,"Clouds... ");
	IF_CN() ShowStringCN(0,0,"�Ʋ�...");

	InitCloudEffect(5);
	SetWorkMode(MODE_CLUDE_VEIW);
	OS_UPDATA_OLED();
}

//�����¶ȵ���ת��
void AdjFanByTmp(void)
{
	u8 speed;
	static u8 fanEnable;
	
	if (sysInfo.temperature > START_TMP+2)
	{
		fanEnable = 1;
	}

	if (sysInfo.temperature  < START_TMP)
	{
		SetFanSpeed(0);//�ر�
		fanEnable = 0;
	}
	else if (sysInfo.temperature  > MAX_TMP)
	{
		SetFanSpeed(100);
	}
	else
	{
		if (fanEnable)
		{
			speed = 20 + (sysInfo.temperature -START_TMP)*10;
			SetFanSpeed(speed);
		}
	}

	if (sysInfo.temperature == 0) //0��,˵���¶��쳣,ǿ��ת��
	{
		SetFanSpeed(80);
	}
}

//LEDָʾ����˸
//�㷨:0000 0111,120ms���һ��λ�����Ϊ1������������  
void LedIndicatorFlash()
{
#define LED_SPEED	2
	static u8 cnt = 0;
	u16 inc;

	if (sysFlag.Bits.LED_sta)  //����ָʾ��
	{
//		inc = 0x01 << cnt/LED_SPEED %16; //����

//		if (sysInfo.ledIndicator & inc)
//			LED0 = 0;
//		else 
//			LED0 = 1;

//		cnt++;
		
		//if(connectingWiFiFlag == 1)		//������WiFi
		if((wifiConnetFlag == 0)&&(sysFlag.Bits.WIFI_MODE==1))
		{
					
			LED_WIFI = 1;  
			cnt ++;
			if(cnt > 10)
			{
				cnt = 0;
				LED0 ^= 1;
			}
		}
		else		//��������
		{
			LED0 = 0;
			LED_WIFI = 0; //WIFIָʾ��
		}
		
	}
	else
	{
		LED0 = 1; //����
		LED_WIFI = 1;  
	}
}

//����������
//�㷨:0000 0111,120ms���һ��λ�����Ϊ1������������  
void BeepSong(void)
{
#define BEEP_SPEED	20
	static u16 cnt;

	if (cnt++ >= BEEP_SPEED)
	{
		cnt = 0;

		if (sysInfo.Beep_type)
		{
			if (sysInfo.Beep_type & 0x80)BEEP_PIN = 1;
			else BEEP_PIN = 0;
			sysInfo.Beep_type <<=1;//����1λ
		}
		else
		{
			BEEP_PIN = 0;
		}
	}
}
//�����������
/*
�����߼���
1����1~3�� ����WIFI INFOģʽ��ָʾ�������£���ģ������·������ָʾ�Ʊ�Ϊ����
2����4~8�� ָʾ�Ʊ�Ϊ����������WIFIΪAPģʽ
3����8�����ϣ�ָʾ�������£�����WIFIΪSTAģʽ
*/
void CheckUdataKey(void)
{
	static u8 keyDnCnt=0;

//-----
	
	if (KEY_Updata != 0) //���°���
	{
		sysFlag.Bits.LED_sta = 1; //����ָʾ��
		keyDnCnt++;
		//sysInfo.ledIndicator = 0x55;
		SetLedIndicator(LED_ITR_FLASH);
		if (keyDnCnt > 3 ) //�ָ���������
		{
//			LED0 = 0;
			SetPwmA(0,0,0,0,0,0);		//�صƣ���ʾ�û���ִ���˻ָ���������
			sysFlag.Bits.WIFI_MODE = 2;	//WIFI APģʽ
			STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //����
			keyDnCnt = 0;
			sysFlag.Bits.LED_SAVE = 1;
			ESP8266_LoadDefault();
			delay_ms(100);
			ESP8266_SetAPmode("", "");
			SystemLoadDefault(); //�ָ�Ĭ��
			SoftReset();
		}
	}
	else
	{
		
		keyDnCnt = 0;
	}

	//ָʾ����˸��ʽ
	//if (keyDnCnt > 2)
	//{
	//	SetLedIndicator(LED_ITR_FLASH);
	//}
	//else if (keyDnCnt > 10)
	//{
	//	SetLedIndicator(LED_ITR_TWO);
	//}
}

//��ص�
void LedOnOff(void)
{

	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"     Power   ");
			ShowStringCN(0,1,"    On  Off   ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"   �رյƹ�?  ");
			ShowStringCN(0,1,"    ��  �� ");
		}

		if (saveInfo == SAVE_YES)
		{
		  	IF_EN() OLED_ShowStringA(64,16,"Off",0);
			IF_CN() OLED_FLIP(64,1,16);
		}
		else 
		{
			IF_EN() OLED_ShowStringA(32,16,"On",0);
			IF_CN() OLED_FLIP(32,1,16);
		}
		break;
	case 1: //������OK��
		if (saveInfo == SAVE_YES) //ѡ�����ON
		{
			SetPwmA(0,0,0,0,0,0); //�ص�
		//	UartSendPwmMsg();
			IF_EN() ShowStringCN(0,0,"   Power off");
			IF_CN() ShowStringCN(0,0,"  �ƹ��ѹر�");
			SetWorkMode(MODE_POWER_OFF);
		}
		else
		{
			IF_EN() ShowStringCN(0,0,"   Power on");
			IF_CN() ShowStringCN(0,0,"  �ƹ��ѿ���");
			SetWorkMode(MODE_TIMER);
		}
		subInfo.step = 0;  //���
		break;
	}
	OS_UPDATA_OLED();

//	if (sysInfo.workmode == MODE_ON_OFF) //�Ѿ��ǹص�
}

//�����л�
void Language(void)
{
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&sysInfo.language,0,1);
		ShowString(0,0," English");
		ShowStringCN(0,1," ����");

		IF_EN() OLED_ShowStringA(0,0,">",1);
		IF_CN() OLED_ShowStringA(0,16,">",1);
		break;
	case 1: //������OK��
		IF_EN() 
		{
			ShowStringCN(0,0,"Language:");
			ShowStringCN(0,1,"English");
		}
		IF_CN() 
		{
			ShowStringCN(0,0,"�������л���:");
			ShowStringCN(0,1,"����");
		}
		subInfo.step = 0;  //���
		saveInfo = SAVE_LANGUAGE;
		break;
	}
	OS_UPDATA_OLED();
}

//ָʾ�ƿ���
void PilotLight(void)
{
	subInfo.maxSetp = 1;
	
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //��ʾ��ʾ
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"  Pilot Light ");
			ShowStringCN(0,1,"    On  Off   ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"  �ر�ָʾ��? ");
			ShowStringCN(0,1,"    ��  �� ");
		}

		if (saveInfo == SAVE_YES)
		{
			IF_EN() OLED_ShowStringA(64,16,"Off",0);
			IF_CN() OLED_FLIP(64,1,16);
		}
		else 
		{
			IF_EN() OLED_ShowStringA(32,16,"On",0);
			IF_CN() OLED_FLIP(32,1,16);
		}
		break;
	case 1: //������OK��
		if (saveInfo == SAVE_YES) //ѡ�����ON
		{
			sysFlag.Bits.LED_sta = 0;		
			IF_EN() ShowStringCN(0,0,"    Off   ");
			IF_CN() ShowStringCN(0,0,"ָʾ���ѹر�");
		}
		else
		{
			sysFlag.Bits.LED_sta = 1;	
			IF_EN() ShowStringCN(0,0,"   On    ");
			IF_CN() ShowStringCN(0,0,"ָʾ���ѿ���");
		}
		subInfo.step = 0;  //���
		sysFlag.Bits.LED_SAVE = 1;

		break;
	}
	OS_UPDATA_OLED();
}

//��������
void saveLanguage(void)
{
	u16 temp;
	temp = sysInfo.language;
	STM_FLASH_WRITE(FLASH_LANGUAGE_ADDR,(u16*)&temp,sizeof(temp)); //��������
}

extern u16 oledFrameSN; //�������IAPPROC��

//��ʾ��������
void ShowUpdataProgress(void)
{
	OLED_Clear_Ram();
	ShowString(0,0,"Updata Fireware");
	ShowString(0,1,"Recver:");

	/*
	IF_CN() 
	{
		ShowStringCN(0,0,"�����̼�");
		ShowStringCN(0,1,"�ѽ���:");
	}
	*/
	OLED_ShowNum(56,16,oledFrameSN,4,FONT16);
}

void ErasureUserLogo(void)
{
	ErasureImg128X32();
}
void DisplayUserLogo(void)
{
	ShowImg128X32(gImage_LOGO);
}


//-----------end of file--------------

