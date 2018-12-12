/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 系统操作功能
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

_calendar_obj now;  //时间
TIMER manualPwm={1,10,10,15,15,15,15,15,15}; //手动调光

void (*ClockFunc)(void) = NullFucn; //每秒要执行的函数功能

u8 saveInfo; //保存信息
u16 timeBuf; //存储时间
u8 previewTyp=0; //预览类型
u8 wifiInfo=0; //1:WIFI已连接 0:WIFI未连接

SYSINFO sysInfo;
SYSFLAG sysFlag={0x0000};

u8 CPU_GUID[12]; //96位芯片唯一标识

MotorInfo_s motorInfo; //电机相关信息

//uint8_t  connectingWiFiFlag = 0;	//正在连接WiFi路由器指示灯
		
//预先设定好的电机控制信息参数
const MotorInfo_s presetMotor = //电机相关信息
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


#define SAVE_TIME	0x01	//保存时间
#define SAVE_TIMER_SET	0x02	//保存定时设置
#define SAVE_MANUAL 0x04	//手动模式
#define SAVE_EFFECT	0x08	//灯光效果
#define SAVE_YES	0x10	//YES和NO选择
#define SAVE_PREVIEW	0x20 //定时预览
#define SAVE_LANGUAGE	0x40 //保存语言

//空函数
void NullFucn(void){}

//读取Easy Timer的值
void ReadEasyTimer()
{
	if(!(saveInfo & SAVE_TIMER_SET))  //从菜单进入，要读取数据
	{
		saveInfo |= SAVE_TIMER_SET;
		STM_FLASH_READ(FLASH_EASY_TIMER,(u16*)&esayTimer,sizeof(esayTimer)); //读取EASY 定时信息
	}
}

//显示3路或6路PWM界面
void ShowPwmUI(u8 p1,u8 p2,u8 p3,u8 p4,u8 p5,u8 p6,u8 index,u8 page)
{
	OLED_Clear_Ram();
	if (sysInfo.pwmNumber == 3) //3路
	{
		OLED_ShowString(0,16,"   %     %     %");
		//显示内容

		IF_EN()ShowStringCN(0,0,"  A     B     C ");
		IF_CN()ShowStringCN(0,0," A路   B路   C路");
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
	else  //--------------------6路
	{
		OLED_ShowString(0,16,"   %     %     %");

		//显示内容
		switch(page)
		{
		case 0:
			IF_EN()ShowStringCN(0,0,"  A     B     C ");
			IF_CN()ShowStringCN(0,0," A路   B路   C路");
			OLED_ShowNum( 0,16,p1,3,16);
			OLED_ShowNum(48,16,p2,3,16);
			OLED_ShowNum(96,16,p3,3,16);
			break;

		case 1:
			IF_EN()ShowStringCN(0,0,"  D     E     F ");
			IF_CN()ShowStringCN(0,0," D路   E路   F路");
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
	} //6路

	OS_UPDATA_OLED();
}

//日出设置
void SunriseSet(void)
{
	subInfo.maxSetp = 1;

	ReadEasyTimer();

	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Set Sunrise time");
	IF_CN() ShowStringCN(0,0," 设置日出时间");

	ShowString(5,1,"10:30");
	ShowTwoNum(5,1,esayTimer.s_hour,1);
	ShowTwoNum(8,1,esayTimer.s_minute,1);

	switch(subInfo.step)
	{
	case 0:  //时
		add_dec_u8(&esayTimer.s_hour,12,1,TRUE);   //限定日出时间范围在1~12点
		ShowTwoNum(5,1,esayTimer.s_hour,0);
		break;
	case 1:  //分
		add_dec_u8((u8*)&esayTimer.s_minute,59,0,TRUE); 
		ShowTwoNum(8,1,esayTimer.s_minute,0);		
		break;
	}
	OS_UPDATA_OLED();
}

//日落设置
void SundownSet(void)
{
	subInfo.maxSetp = 1;

	ReadEasyTimer();
	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Set Sunset time");
	IF_CN() ShowStringCN(0,0," 设置日落时间");

	ShowString(5,1,"10:30");
	ShowTwoNum(5,1,esayTimer.e_hour,1);
	ShowTwoNum(8,1,esayTimer.e_minute,1);

	switch(subInfo.step)
	{
	case 0:  //时
		add_dec_u8(&esayTimer.e_hour,23,13,TRUE); 
		ShowTwoNum(5,1,esayTimer.e_hour,0);
		break;
	case 1:  //分
		add_dec_u8((u8*)&esayTimer.e_minute,59,0,TRUE); 
		ShowTwoNum(8,1,esayTimer.e_minute,0);		
		break;
	}
	OS_UPDATA_OLED();
}

//亮度设置
void SunlightSet(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp  = 6;
	ReadEasyTimer();

	//
	if (sysInfo.pwmNumber == 3) //3路
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmDay.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
		}
		else	
		{
			SetKeyStaue(keyClear_flag); //3路界面不能翻页
		}
		
		esayTimer.pwmDay.pwm4 = 0;
		esayTimer.pwmDay.pwm5 = 0;
		esayTimer.pwmDay.pwm6 = 0;
		page = 0;
	}
	else  //--------------------6路
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
			add_dec_u8(&page,1,0,TRUE);  //6路,进入手动模式，按上下键可翻页	
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

//月光亮度设置
void MoonlightSet(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp  = 6;
	ReadEasyTimer();

	if (sysInfo.pwmNumber == 3) //3路
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step) 
		{
			pPwm = &esayTimer.pwmMoon.pwm1+(subInfo.step-1);
			add_dec_u8(pPwm,100,0,TRUE);
		}
		else	
		{
			SetKeyStaue(keyClear_flag); //3路界面不能翻页
		}

		page = 0;

		//另外3路设为0
		esayTimer.pwmMoon.pwm4 = 0;
		esayTimer.pwmMoon.pwm5 = 0;
		esayTimer.pwmMoon.pwm6 = 0;

	}
	else  //--------------------6路
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
			add_dec_u8(&page,1,0,TRUE);  //6路,进入手动模式，按上下键可翻页	
		}
	} //6路

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

//保存手动定时设置
void SaveEasyTimer(void)
{
	u8 i;
	u16 t; //时间
	u8 hour,munute;
	for (i=0;i<=PRGM_STEP;i++) //先关掉所有的定时
	{
		TimerProg[i].enable = 0;
	}

	//日出,日出时间点的亮度是月光的亮度
	TimerProg[0].enable = 1;
	TimerProg[0].hour = esayTimer.s_hour;
	TimerProg[0].minute = esayTimer.s_minute;
	TimerProg[0].pwm1 = esayTimer.pwmMoon.pwm1;
	TimerProg[0].pwm2 = esayTimer.pwmMoon.pwm2;
	TimerProg[0].pwm3 = esayTimer.pwmMoon.pwm3;
	TimerProg[0].pwm4 = esayTimer.pwmMoon.pwm4;
	TimerProg[0].pwm5 = esayTimer.pwmMoon.pwm5;
	TimerProg[0].pwm6 = esayTimer.pwmMoon.pwm6;

	//日出时长
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

	//日落
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

	//月光
	TimerProg[3].enable = 1;
	TimerProg[3].hour = esayTimer.e_hour;
	TimerProg[3].minute = esayTimer.e_minute;
	TimerProg[3].pwm1 = esayTimer.pwmMoon.pwm1;
	TimerProg[3].pwm2 = esayTimer.pwmMoon.pwm2;
	TimerProg[3].pwm3 = esayTimer.pwmMoon.pwm3;
	TimerProg[3].pwm4 = esayTimer.pwmMoon.pwm4;
	TimerProg[3].pwm5 = esayTimer.pwmMoon.pwm5;
	TimerProg[3].pwm6 = esayTimer.pwmMoon.pwm6;

	STM_FLASH_WRITE(FLASH_TIMER_ADDR,(u16*)&TimerProg,sizeof(TimerProg)); //保存定时信息
	STM_FLASH_WRITE(FLASH_EASY_TIMER,(u16*)&esayTimer,sizeof(esayTimer)); //保存EASY 定时信息
	//保存手动定时信息
}

/*-----------------------------------
函  数：手动模式
功  能：
输  入：
输  出：
-----------------------------------*/
void ManualSetMenu(void)
{
	u8* pPwm;
	static u8 page;

	subInfo.maxSetp = 7;

	SetWorkMode(MODE_MUNUAL); //PWM变为手动模式

	if (!(saveInfo & SAVE_MANUAL))  //从菜单进入，要读取数据
	{
		saveInfo |= SAVE_MANUAL;
		STM_FLASH_READ(FLASH_MANUAL_PWM, (u16*)&manualPwm, sizeof(manualPwm)); //读取手动的PWM
	}


	if (sysInfo.pwmNumber == 3) //3路
	{
		if (subInfo.step > 3)subInfo.step = 1;

		if (subInfo.step)
		{
			pPwm = &manualPwm.pwm1 + (subInfo.step - 1);
			add_dec_u8(pPwm, 100, 0, TRUE);
		}
		else
		{
			SetKeyStaue(keyClear_flag); //3路界面不能翻页
		}

		page = 0;

		//另外3路设为0
		manualPwm.pwm4 = 0;
		manualPwm.pwm5 = 0;
		manualPwm.pwm6 = 0;

	}
	else  //--------------------6路
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
			add_dec_u8(&page, 1, 0, TRUE);  //6路,进入手动模式，按上下键可翻页	
		}
	} //6路

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
函  数：显示当前PWM亮度
功  能：
输  入：
输  出：
-----------------------------------*/
void ShowCurrentPwmMenu(void)
{
	u8 temp;
	SetClockFun(ShowCurrentPwmMenu); //定时显示
	OLED_Clear_Ram();

	//显示内容
	if (sysInfo.pwmNumber == 3) //3路
	{
		IF_EN()ShowStringCN(0,0,"  A     B     C ");
		IF_CN()ShowStringCN(0,0," A路   B路   C路");
		OLED_ShowString(0,16,"   %     %     %");
		OLED_ShowNum( 0,16,GetPwm(1),3,16);
		OLED_ShowNum(48,16,GetPwm(2),3,16);
		OLED_ShowNum(96,16,GetPwm(3),3,16);
	}
	else  //6路
	{
		OLED_ShowString(0,0, "A:");
		OLED_ShowString(42,0,"B:");
		OLED_ShowString(88,0,"C:");
		OLED_ShowString(0,16, "D:");
		OLED_ShowString(42,16,"E:");
		OLED_ShowString(88,16,"F:");

		temp = GetPwm(1);
		if (temp > 99)OLED_ShowNum(14,0,temp,3,16); //3位
		else OLED_ShowNum(14,0,temp,2,16);

		temp = GetPwm(2);
		if (temp > 99)OLED_ShowNum(58,0,temp,3,16); //3位
		else OLED_ShowNum(58,0,temp,2,16);

		temp = GetPwm(3);
		if (temp > 99)OLED_ShowNum(102,0,temp,3,16); //3位
		else OLED_ShowNum(102,0,temp,2,16);

		temp = GetPwm(4);
		if (temp > 99)OLED_ShowNum(14,16,temp,3,16); //3位
		else OLED_ShowNum(14,16,temp,2,16);

		temp = GetPwm(5);
		if (temp > 99)OLED_ShowNum(58,16,temp,3,16); //3位
		else OLED_ShowNum(58,16,temp,2,16);

		temp = GetPwm(6);
		if (temp > 99)OLED_ShowNum(102,16,temp,3,16); //3位
		else OLED_ShowNum(102,16,temp,2,16);

	}

	OS_UPDATA_OLED();
}

//帮助
void Help(void)
{
	ShowString(0,0,"      Help    ");
	ShowString(0,1,"==============");
	OS_UPDATA_OLED();
}

//日期时间设置
void SetDataTime(void)
{
	static u8 year;

	subInfo.maxSetp = 5;

	if(!(saveInfo & SAVE_TIME)) //从菜单进入，重新读取时间
	{
		saveInfo |= SAVE_TIME; //标记
		//获取当前时间
		now.hour = calendar.hour;
		now.min = calendar.min;
		now.sec = calendar.sec;
		now.w_date = calendar.w_date;
		now.w_month = calendar.w_month;
		year = calendar.w_year-2000; //年份是u16，要转成8位的，2015-2000 =15
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
	case 0:  //年
		add_dec_u8(&year,99,0,TRUE); 
		ShowTwoNum(2,0,year,0);
		break;
	case 1:  //月
		add_dec_u8((u8*)&now.w_month,12,0,TRUE); 
		ShowTwoNum(5,0,now.w_month,0);		
		break;
	case 2:  //日
		add_dec_u8((u8*)&now.w_date,31,0,TRUE);  //这里没有区分大小月和润月
		ShowTwoNum(8,0,now.w_date,0);
		break;
	case 3: //小时
		add_dec_u8((u8*)&now.hour,23,0,TRUE);
		ShowTwoNum(0,1,now.hour,0);
		break;
	case 4: //分钟
		add_dec_u8((u8*)&now.min,59,0,TRUE);
		ShowTwoNum(3,1,now.min,0);
		break;
	case 5: //秒
		add_dec_u8((u8*)&now.sec,59,0,TRUE);
		ShowTwoNum(6,1,now.sec,0);
		break;
	}
	now.w_year = 2000+year; //把年再转回来

	OS_UPDATA_OLED();
}

//恢复默认
void LoadDefault(void)
{
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"Load default ?  ");
			ShowStringCN(0,1,"    NO  YES     ");
		}

		IF_CN()
		{
			ShowStringCN(0,0,"恢复默认设置?");
			ShowStringCN(0,1,"    否  是   ");
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
	case 1: //按下了OK键
		if (saveInfo == SAVE_YES) //选择的是YES
		{
			SystemLoadDefault(); //恢复默认
			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Press ESC key  ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"恢复成功！");
				ShowStringCN(0,1,"请按ESC键返回");
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
				ShowStringCN(0,0,"未恢复！");
				ShowStringCN(0,1,"请按ESC键返回");
			}
			
		}
		subInfo.step = 0;  //清除
		break;
	}
	OS_UPDATA_OLED();
}

//重置系统默认值
void SystemLoadDefault(void)
{
	STM_FLASH_WRITE(FLASH_TIMER_ADDR,(u16*)&presetTimer,sizeof(presetTimer)); //保存定时信息
	STM_FLASH_WRITE(FLASH_MANUAL_PWM,(u16*)&manualPwm,sizeof(manualPwm)); //保存手动PWM信息
	STM_FLASH_WRITE(FLASH_EASY_TIMER,(u16*)&presetEsayTimer,sizeof(presetEsayTimer)); //保存EASY 定时信息
	STM_FLASH_WRITE(FLASH_EFFECT_ADDR,(u16*)&weather_data,sizeof(weather_data)); //保存天气信息	
	STM_FLASH_WRITE(FLASH_MOTOR_INFO_ADDR, (u16*)&presetMotor, sizeof(presetMotor));	//保存电机数据
	//STM_FLASH_WRITE(FLASH_MOONLIGHT_ADDR, (u16*)&moonling_data, sizeof(moonling_data));	//保存月相默认数据
	//STM_FLASH_WRITE(FLASH_LANGUAGE_ADDR,(u16*)&sysInfo.language,sizeof(sysInfo.language)); //保存语言
	// STM_FLASH_WRITE(FLASH_INFO_ADDR,(u16*)&Sysflag,sizeof(Sysflag)); //系统信息
}


//版本
void Version(void)
{
	u16 temp;
	OLED_Clear_Ram();
//	IF_EN()	ShowStringCN(0,0,"Software version");
//	IF_CN() ShowStringCN(0,0,"  软件版本信息  ");

	ShowStringCN(0,0," ver:xx-xx      ");
	ShowStringCN(0,1,"code:xx-xx      ");

	ShowTwoNum(5,0,version[0],1);
	ShowTwoNum(8,0,version[1],1);

	temp = GetCustomCode();

	ShowTwoNum(5,1,temp>>8,1);
	ShowTwoNum(8,1,(u8)temp,1);
	OS_UPDATA_OLED();
}

//设置预览的时间
void SetPreveiwTime(u8 hour,u8 minute)
{
	timeBuf = hour*60+minute;
	previewTyp = 1;
}

//定时预览
void PreviewTimer(void)
{
#define DAY_MINUTE	1439  //24*60 一天的分钟数
	u8 hour,minute;
	static u8 cnt100ms;

	SetWorkMode(MODE_PREVIEW);
	if (++cnt100ms >5)cnt100ms = 0; //50ms一次
	else return;

//	SetClockFun(PreviewTimer); //每秒都执行
	saveInfo |= SAVE_PREVIEW;

	if (timeBuf >= DAY_MINUTE)  //走完就停止
	{
		if (previewTyp) //WIFI控制下的预览,要自动回到定时模式
		{
		//	timeBuf = 0;
			previewTyp = 0;
			SetWorkMode(MODE_TIMER);
		}
	//	timeBuf = DAY_MINUTE;
	}
	else
	{
		timeBuf += 1; //1秒钟相当于走5分钟
		hour = timeBuf / 60;
		minute = timeBuf % 60;

		SetPwmSpeed(PWM_SPEED_PREVIEW);
		SetPwmByTimer(hour,minute);
	}

	OLED_Clear_Ram();
	IF_EN()	ShowStringCN(0,0," Timer preview");
	IF_CN() ShowStringCN(0,0,"    定时预览");
	ShowString(0,1,"10:00:00[00->01]");
	ShowTwoNum(0,1,hour,1);
	ShowTwoNum(3,1,minute,1);
	ShowTwoNum(9,1,sysInfo.indexTimer+1,1); //显示时间点1 
	ShowTwoNum(13,1,sysInfo.nextTimer+1,1); //显示时间点2
	
	OS_UPDATA_OLED();
}

//重置定时预览
void ResetPreviewTimer(void)
{
	timeBuf = 0;
	previewTyp=0;
}

//桌面时钟
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
		IF_CN() ShowStringCN(0,1,"  定时[00->01]");
		ShowTwoNum(7,1,sysInfo.indexTimer+1,1); //显示时间点1 
		ShowTwoNum(11,1,sysInfo.nextTimer+1,1); //显示时间点2
		break;

	case MODE_MUNUAL:
		IF_EN() ShowString(3,1,  "[ Dimmer ]");
		IF_CN() ShowStringCN(3,1,"[  手动  ]");
		break;

	case MODE_CLUDE:
		IF_EN() ShowString(3,1,  "[  Cloud ]");
		IF_CN() ShowStringCN(3,1,"[  云彩  ]");
		break;
	case MODE_FLASH: //灯光特效
		IF_EN() ShowString(3,1,  "[  Flash ]");
		IF_CN() ShowStringCN(3,1,"[  闪电  ]");
		break;
	case MODE_CLUDE_VEIW:
		IF_EN() ShowString(3,1,  "[  Cloud ]");
		IF_CN() ShowStringCN(3,1,"[云彩预览]");
		break;
	case MODE_FLASH_VEIW: //灯光特效
		IF_EN() ShowString(3,1,  "[  flash ]");
		IF_CN() ShowStringCN(3,1,"[闪电预览]");
		break;
	case MODE_MOONLIGHT:
		IF_EN() ShowString(3, 1, "[  Moon  ]");
		IF_CN() ShowStringCN(3, 1,"[  月相  ]");
		break;
	case MODE_MOONLIGHT_VEIW:
		IF_EN() ShowString(3, 1, "[  Moon  ]");
		IF_CN() ShowStringCN(3, 1, "[月相预览]");
		break;
	case MODE_POWER_OFF:
		IF_EN() ShowString(2,1,  "[Power off]");
		IF_CN() ShowStringCN(3,1,"[  关灯  ]");
		break;
	}

//	OS_UPDATA_OLED();
}

//显示温度
void TempFan(void)
{
	u8 temp;
	u8 speed;
	SetClockFun(TempFan); //定时显示

	temp = GetTemperature()/10; 
	OLED_Clear_Ram();
	IF_EN() ShowStringCN(0,0,"Temperature:00 C");
	IF_CN() ShowStringCN(0,0,"   当前温度:00 C");
	if (temp > 99)ShowString(12,0,"??"); //超过99度 显示??
	else ShowTwoNum(12,0,temp,1);

	speed = GetFanSpeed();
	IF_EN() ShowStringCN(0,1,"Fan speed  :xx %");
	IF_CN() ShowStringCN(0,1,"   风扇转速:xx %");
	if (speed == 100)ShowString(12,1,"100"); //显示100%
	else ShowTwoNum(12,1,speed,1);

	OS_UPDATA_OLED();
}

//Wifi配置为AP模式
#if 0
void WifiApModeConfig(void)
{
	//按OK键确认，saveinfo用来保存选择的是YES还是NO
	subInfo.maxSetp = 1; 
	
	OLED_Clear_Ram();
	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"Set to AP mode ?");
			ShowStringCN(0,1,"    NO  YES     ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"配置成热点模式?");
			ShowStringCN(0,1,"    否  是     ");
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
	case 1: //按下了OK键
		if (saveInfo == SAVE_YES) //选择的是YES
		{
			sysFlag.Bits.LED_sta = 1; //开启指示灯
			sysFlag.Bits.LED_SAVE = 1;

			IF_EN()
			{
				ShowStringCN(0,0,"Config Wifi   ");
				ShowStringCN(0,1,"Waiting...    ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"正在配置Wifi");
				ShowStringCN(0,1,"请稍等...  ");
			}
			
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_SetAPmode("","");
			SPI_Flash_Write((u8*)&M30_Buf,W25QXX_MAC,6);//保存MAC地址，MAC地址只有6位
			SetUartRxType(USART_TYPE_LED);

			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Reset...     ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"配置成功！");
				ShowStringCN(0,1,"正在重启系统...");
			}
			
			OS_UPDATA_OLED();
			delay_ms(2000);
			SoftReset(); //重启
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
				ShowStringCN(0,0,"请按ESC键返回");
			}			
		}
		break;
	}
	OS_UPDATA_OLED();

}
#else
void WifiApModeConfig(void)
{
	//按OK键确认，saveinfo用来保存选择的是YES还是NO
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();
	if (subInfo.step == 0)
	{
		IF_EN()
		{
			OLED_ShowString(0, 8, "Run in Wifi mode");
			sysFlag.Bits.LED_sta = 1; //开启指示灯
			sysFlag.Bits.LED_SAVE = 1;
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_SetAPmode("", "");
			SPI_Flash_Write((u8*)&M30_Buf, W25QXX_MAC, 6);//保存MAC地址，MAC地址只有6位
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
//Wifi配置为STA模式
void WifiStaModeConfig(void)
{
	subInfo.maxSetp = 1;

	OLED_Clear_Ram();
	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowString(0,0,"Set to STA mode?");
			ShowString(0,1,"    NO  YES     ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"配置成网络模式?");
			ShowStringCN(0,1,"    否  是     ");
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
	case 1: //按下了OK键
		if (saveInfo == SAVE_YES) //选择的是YES
		{
			sysFlag.Bits.LED_sta = 1; //开启指示灯
			sysFlag.Bits.LED_SAVE = 1;

			IF_EN()
			{
				ShowStringCN(0,0,"Config Wifi   ");
				ShowStringCN(0,1,"Waiting...    ");
			}

			IF_CN()
			{
				ShowStringCN(0,0,"正在配置Wifi");
				ShowStringCN(0,1,"请稍等...  ");
			}
			OS_UPDATA_OLED();
			SetUartRxType(USART_TYPE_AT);
			ESP8266_LoadDefault(); //恢复WIFI默认
			SetUartRxType(USART_TYPE_LED);
			IF_EN()
			{
				ShowStringCN(0,0,"Successfully ");
				ShowStringCN(0,1,"Reset...     ");
			}
			IF_CN()
			{
				ShowStringCN(0,0,"配置成功！");
				ShowStringCN(0,1,"正在重启系统...");
			}
			OS_UPDATA_OLED();
			delay_ms(2000);
			SoftReset(); //重启
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
				ShowStringCN(0,0,"请按ESC键返回");
			}
		}
		break;
	}
	OS_UPDATA_OLED();
}

//配置STA连接网络
void ConfigWifiStaConn(void)
{
	SetUartRxType(USART_TYPE_AT);
	conTowifi();
	SetUartRxType(USART_TYPE_LED);
}

//WIFI信息
void WifiInfo(void)
{
	u8 temp_IP[17]={0};
	OLED_Clear_Ram();
	//SetClockFun(WifiInfo);		//取消每秒都去显示WIFI信息，否则影响按键的灵敏度
	SetUartRxType(USART_TYPE_AT);

	sysFlag.Bits.LED_sta = 1; //开启指示灯

	IF_EN()
	{
		ShowStringCN(0,0,"Info Reading    ");
		ShowStringCN(0,1,"Please Wait.....");
	}

	IF_CN()

	{
		ShowStringCN(0,0,"查看中          ");
		ShowStringCN(0,1,"请稍等......    ");
	}

	OS_UPDATA_OLED();
	//if(!HLK_M30_EnterATmode()) //这里进入AT模式要改
	{
		OLED_Clear_Ram();
		ESP8266_GetIp();
		memcpy(temp_IP,M30_Buf,sizeof(temp_IP));
		if (temp_IP[0] == '0') //没有取到IP
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
				ShowStringCN(0,0,"Wifi未连接");
				ShowStringCN(0,1,"等待配置中...");
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
//		IF_CN() ShowStringCN(0,0,"Wifi读取失败!");
//	}
	//HLK_M30_ExitATmode(); //退出AT模式
	SetUartRxType(USART_TYPE_LED);

	if (wifiInfo == 0x81) //成功获取IP就退出
	{
		SetClockFun(Desktop);
		wifiInfo &= ~0x80;
		SetBeepType(BEEP_THREE);//获取到IP就响三下
	}
	OS_UPDATA_OLED();
}

//退出从操作界面返回菜单要处理的事
void ReturnToMenu(void)
{
	if (saveInfo & SAVE_TIME) //保存时间
	{
		saveInfo &= ~SAVE_TIME;
		RTC_Set(
			now.w_year,
			now.w_month,
			now.w_date,
			now.hour,
			now.min,
			now.sec
			); //设置时间
	}
	else if (saveInfo & SAVE_MANUAL) //保存手动PWM
	{
		saveInfo &= ~SAVE_MANUAL;
		STM_FLASH_WRITE(FLASH_MANUAL_PWM,(u16*)&manualPwm,sizeof(manualPwm)); //保存PWM信息
		//workmode &= ~MODE_MUNUAL;
		SetWorkMode(MODE_TIMER);
	}
	else if (saveInfo & SAVE_TIMER_SET)//保存定时
	{
		saveInfo &= ~SAVE_TIMER_SET;
		SaveEasyTimer();
	}
	else if (saveInfo & SAVE_EFFECT)//退出效果模式
	{
		saveInfo &= ~SAVE_EFFECT;
		SetWorkMode(MODE_TIMER);
	}
	else if (saveInfo & SAVE_YES) //YES和NO
	{
		saveInfo &= ~SAVE_YES;
	}
	else if (saveInfo &SAVE_PREVIEW)
	{
		saveInfo &= ~SAVE_PREVIEW;
		SetWorkMode(MODE_TIMER);
		timeBuf = 0;
	}
	else if (saveInfo & SAVE_LANGUAGE) //语言
	{
		saveInfo &= ~SAVE_LANGUAGE;
		saveLanguage();
	}

	SetClockFun(NullFucn); //清除秒显示

	SetLedIndicator(LED_ITR_ON); //指示灯长亮
}

//显示0~99的两位数
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


//设置客户代码
void SetCustomCode(u16 code1)
{	
	SPI_Flash_Write((u8*)&code1,W25QXX_CUSTOM_CODE,sizeof(code1));//写入客户标记代码
}

//获取客户代码
u16 GetCustomCode(void)
{	
	u16 code1;
	SPI_Flash_Read((u8*)&code1,W25QXX_CUSTOM_CODE,sizeof(code1));
	return code1;
}

//设置调光路数
void SetPwmNumber(u8 num)
{
	SPI_Flash_Write((u8*)&num,W25QXX_PWM_NUM,sizeof(num));
}

//获取调光路数
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


//获取GUID，index = 0~11
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

//读取GUID到数组中
void ReadGUID(void)
{
	/*  读CPU的GUID
	u8 i;
	u8 *p;
	u8 temp;
	p = CPU_GUID;
	for(i=0;i<12;i++)
	{
		temp =(u8)*(vu32*)(0x1FFFF7E8+i);
		if(temp > 0x80)temp = 0xFF-temp; //确保CPUID小于0x80,不会与命令冲突
		*p++ = temp;
	}
	*/

	//读MAC
	//SPI_Flash_Read((u8*)&CPU_GUID,W25QXX_MAC,6);
}

//观看闪电效果
void ViewFlashEffect(void)
{
	if(!(saveInfo & SAVE_EFFECT))  //从菜单进入，要读取数据
	{
		saveInfo |= SAVE_EFFECT;
	}
	OLED_Clear_Ram();
	
	IF_EN() ShowStringCN(0,0,"Lightning...");
	IF_CN() ShowStringCN(0,0,"闪电中...");

	InitFlashEffect(20);
	SetWorkMode(MODE_FLASH_VEIW);
	OS_UPDATA_OLED();
}

//观看云彩效果
void ViewCloudEffect(void)
{
	if(!(saveInfo & SAVE_EFFECT))  //从菜单进入，要读取数据
	{
		saveInfo |= SAVE_EFFECT;
	}
	OLED_Clear_Ram();

	IF_EN()	ShowStringCN(0,0,"Clouds... ");
	IF_CN() ShowStringCN(0,0,"云彩...");

	InitCloudEffect(5);
	SetWorkMode(MODE_CLUDE_VEIW);
	OS_UPDATA_OLED();
}

//根据温度调节转速
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
		SetFanSpeed(0);//关闭
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

	if (sysInfo.temperature == 0) //0度,说明温度异常,强制转动
	{
		SetFanSpeed(80);
	}
}

//LED指示灯闪烁
//算法:0000 0111,120ms检查一个位，如果为1就亮，否则灭  
void LedIndicatorFlash()
{
#define LED_SPEED	2
	static u8 cnt = 0;
	u16 inc;

	if (sysFlag.Bits.LED_sta)  //开启指示灯
	{
//		inc = 0x01 << cnt/LED_SPEED %16; //左移

//		if (sysInfo.ledIndicator & inc)
//			LED0 = 0;
//		else 
//			LED0 = 1;

//		cnt++;
		
		//if(connectingWiFiFlag == 1)		//在连接WiFi
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
		else		//正常工作
		{
			LED0 = 0;
			LED_WIFI = 0; //WIFI指示灯
		}
		
	}
	else
	{
		LED0 = 1; //不亮
		LED_WIFI = 1;  
	}
}

//蜂鸣器响声
//算法:0000 0111,120ms检查一个位，如果为1就亮，否则灭  
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
			sysInfo.Beep_type <<=1;//左移1位
		}
		else
		{
			BEEP_PIN = 0;
		}
	}
}
//检测升级按键
/*
处理逻辑：
1、按1~3秒 进入WIFI INFO模式，指示灯闪两下，当模块连上路由器后，指示灯变为长亮
2、按4~8秒 指示灯变为快闪，配置WIFI为AP模式
3、按8秒以上，指示灯闪两下，配置WIFI为STA模式
*/
void CheckUdataKey(void)
{
	static u8 keyDnCnt=0;

//-----
	
	if (KEY_Updata != 0) //按下按键
	{
		sysFlag.Bits.LED_sta = 1; //开启指示灯
		keyDnCnt++;
		//sysInfo.ledIndicator = 0x55;
		SetLedIndicator(LED_ITR_FLASH);
		if (keyDnCnt > 3 ) //恢复出厂设置
		{
//			LED0 = 0;
			SetPwmA(0,0,0,0,0,0);		//关灯，提示用户已执行了恢复出厂设置
			sysFlag.Bits.WIFI_MODE = 2;	//WIFI AP模式
			STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //保存
			keyDnCnt = 0;
			sysFlag.Bits.LED_SAVE = 1;
			ESP8266_LoadDefault();
			delay_ms(100);
			ESP8266_SetAPmode("", "");
			SystemLoadDefault(); //恢复默认
			SoftReset();
		}
	}
	else
	{
		
		keyDnCnt = 0;
	}

	//指示灯闪烁方式
	//if (keyDnCnt > 2)
	//{
	//	SetLedIndicator(LED_ITR_FLASH);
	//}
	//else if (keyDnCnt > 10)
	//{
	//	SetLedIndicator(LED_ITR_TWO);
	//}
}

//软关灯
void LedOnOff(void)
{

	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"     Power   ");
			ShowStringCN(0,1,"    On  Off   ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"   关闭灯光?  ");
			ShowStringCN(0,1,"    开  关 ");
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
	case 1: //按下了OK键
		if (saveInfo == SAVE_YES) //选择的是ON
		{
			SetPwmA(0,0,0,0,0,0); //关灯
		//	UartSendPwmMsg();
			IF_EN() ShowStringCN(0,0,"   Power off");
			IF_CN() ShowStringCN(0,0,"  灯光已关闭");
			SetWorkMode(MODE_POWER_OFF);
		}
		else
		{
			IF_EN() ShowStringCN(0,0,"   Power on");
			IF_CN() ShowStringCN(0,0,"  灯光已开启");
			SetWorkMode(MODE_TIMER);
		}
		subInfo.step = 0;  //清除
		break;
	}
	OS_UPDATA_OLED();

//	if (sysInfo.workmode == MODE_ON_OFF) //已经是关灯
}

//语言切换
void Language(void)
{
	subInfo.maxSetp = 1;
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&sysInfo.language,0,1);
		ShowString(0,0," English");
		ShowStringCN(0,1," 中文");

		IF_EN() OLED_ShowStringA(0,0,">",1);
		IF_CN() OLED_ShowStringA(0,16,">",1);
		break;
	case 1: //按下了OK键
		IF_EN() 
		{
			ShowStringCN(0,0,"Language:");
			ShowStringCN(0,1,"English");
		}
		IF_CN() 
		{
			ShowStringCN(0,0,"语言已切换成:");
			ShowStringCN(0,1,"中文");
		}
		subInfo.step = 0;  //清除
		saveInfo = SAVE_LANGUAGE;
		break;
	}
	OS_UPDATA_OLED();
}

//指示灯开关
void PilotLight(void)
{
	subInfo.maxSetp = 1;
	
	OLED_Clear_Ram();

	switch(subInfo.step)   
	{
	case 0: //显示提示
		Choose(&saveInfo,0,SAVE_YES);
		IF_EN()
		{
			ShowStringCN(0,0,"  Pilot Light ");
			ShowStringCN(0,1,"    On  Off   ");
		}
		IF_CN()
		{
			ShowStringCN(0,0,"  关闭指示灯? ");
			ShowStringCN(0,1,"    开  关 ");
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
	case 1: //按下了OK键
		if (saveInfo == SAVE_YES) //选择的是ON
		{
			sysFlag.Bits.LED_sta = 0;		
			IF_EN() ShowStringCN(0,0,"    Off   ");
			IF_CN() ShowStringCN(0,0,"指示灯已关闭");
		}
		else
		{
			sysFlag.Bits.LED_sta = 1;	
			IF_EN() ShowStringCN(0,0,"   On    ");
			IF_CN() ShowStringCN(0,0,"指示灯已开启");
		}
		subInfo.step = 0;  //清除
		sysFlag.Bits.LED_SAVE = 1;

		break;
	}
	OS_UPDATA_OLED();
}

//保存语言
void saveLanguage(void)
{
	u16 temp;
	temp = sysInfo.language;
	STM_FLASH_WRITE(FLASH_LANGUAGE_ADDR,(u16*)&temp,sizeof(temp)); //保存语言
}

extern u16 oledFrameSN; //这个变在IAPPROC中

//显示升级进度
void ShowUpdataProgress(void)
{
	OLED_Clear_Ram();
	ShowString(0,0,"Updata Fireware");
	ShowString(0,1,"Recver:");

	/*
	IF_CN() 
	{
		ShowStringCN(0,0,"升级固件");
		ShowStringCN(0,1,"已接收:");
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

