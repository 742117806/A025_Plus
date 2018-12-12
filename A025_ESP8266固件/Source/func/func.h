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

#ifndef _FUNCTION_H	
#define _FUNCTION_H
//#include "include.h"
#include "sys.h"

void NullFucn(void);
void ManualSetMenu(void);
void Help(void);
void SetDataTime(void);
void LoadDefault(void);
void Version(void);
void PreviewTimer(void);
void Desktop(void);
void WifiApModeConfig(void);
void WifiStaModeConfig(void);
void WifiInfo(void);
void ReturnToMenu(void);
//void TimerSetup(void);
void SunriseSet(void);
void SundownSet(void);
void SunlightSet(void);
void MoonlightSet(void);
void TempFan(void);
void ShowCurrentPwmMenu(void);
void ViewFlashEffect(void);
void ViewCloudEffect(void);
void ResetPreviewTimer(void);
void LedOnOff(void);
void Language(void);//语言切换
void saveLanguage(void);
void PilotLight(void); //LED指示灯

void CheckUdataKey(void); //升级按键功能

void ShowUpdataProgress(void);//升级进度

void SetPreveiwTime(u8 hour,u8 minute);

void SystemLoadDefault(void);//系统默认设置
void AdjFanByTmp(void);

void ConfigWifiStaConn(void); //配置STA模式连网
//-----------------------------------
void SetCustomCode(u16 code1);//设置客户代码
u16 GetCustomCode(void);//获取客户代码

void SetPwmNumber(u8 num);//设置调光路数
u8 GetPwmNumber(void);//获取调光路数

u8 GetGUID(u8 index);//获取GUID，index = 0~11
void ReadGUID(void); //读取GUID到数组中

void LedIndicatorFlash(void); //指示灯闪烁
void BeepSong(void);//蜂鸣器发声

void ErasureUserLogo(void);
void DisplayUserLogo(void);

extern void(*ClockFunc)(void);
#define SetClockFun(X)	ClockFunc=X //设置每秒要执行的函数

#define START_TMP	38  //风扇启动温度
#define MAX_TMP		46  //上限温度 
#define OVER_TMP	68  //过温保护

//系统信息结构体
typedef struct _SysInfo{
	u16 ledIndicator;    //LED指示灯闪烁方式
	u8 workmode;    //工作模式
	u8 indexTimer;  //当前定时点
	u8 nextTimer;   //下一个定时点
	u8 pwmNumber;   //调光路数
	u8 pwmSpeed;	//PWM变化的速度
	u8 language;	//语言 0:英文  1:中文
	u8 updataType;  //升级类型
	u8 temperature;	//灯具温度
	u8 OverProtect;  //过温保护
	u8 MunualFan;	//手动测试FAN
	u8 Beep_type; //喇叭声音类型 
}SYSINFO;


//电机相关信息
typedef struct 
{
	u8 enable;
	u8 s_hour1;
	u8 s_minute1;
	u8 e_hour1;
	u8 e_minute1;
	u8 s_hour2;
	u8 s_minute2;
	u8 e_hour2;
	u8 e_minute2;
	u8 s_hour3;
	u8 s_minute3;
	u8 e_hour3;
	u8 e_minute3;
	u8 angle_h;
	u8 angle_l;
	u8 speed;
	
}MotorInfo_s;

//联合体 位标志
typedef union _flag{
	u16 bytes;
	struct _b{
		u16 LED_sta		:1;  //工作指示灯
	//	u16 LED_wifi		:1;  //WIFI指示灯
		u16 LED_SAVE	:1;  //保存指示灯状态
		uint16_t WIFI_MODE:2;	//WiFi工作模式1STA，2AP，3AP+STA
	}Bits;
}SYSFLAG;

extern SYSINFO sysInfo;
extern SYSFLAG sysFlag;
extern MotorInfo_s motorInfo;		//电机相关信息
//extern uint8_t  connectingWiFiFlag;	//正在连接WiFi路由器指示灯
#define SetWorkMode(X)	sysInfo.workmode=X
#define SetLedIndicator(X)  sysInfo.ledIndicator=X  //设置指示灯闪烁方式
#define SetPwmSpeed(X)	sysInfo.pwmSpeed=X	//设置PWM变化速度

#define SetBeepType(X) sysInfo.Beep_type=X //设置蜂鸣器

#define ENGELISH	0  //英文
#define CHINESE		1  //中文

#define IF_EN()  if(sysInfo.language==ENGELISH)
#define IF_CN()  if(sysInfo.language==CHINESE)

//LED指示灯闪烁方式
#define LED_ITR_OFF  0x0000	//关闭
#define LED_ITR_ONE	 0x0F0F	//1秒闪一次
#define LED_ITR_TWO  0x0009	//闪两下,灭1秒
#define LED_ITR_THREE   0x0049	//闪三下,灭一秒
#define LED_ITR_FLASH	0x5555  //狂闪
#define LED_ITR_ON	 0xFFFF	//一直亮

//蜂鸣器响
#define BEEP_OFF  0x00 //关闭
#define BEEP_ONE  0x80 //响一次
#define BEEP_TWO  0xA0 //响两次
#define BEEP_THREE	0xA8 //响三次

void ShowTwoNum(u8 x,u8 y,u8 num,u8 mode);

#endif
