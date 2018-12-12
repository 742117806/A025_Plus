/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: hlk-m30.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: HLK-M30模块操作
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __HLK_M30_H__
#define __HLK_M30_H__	 
#include "sys.h"
#include "system.h"

//////////////////////////////////////////////////////////////////////////////////	 
#define M30_reset			PAout(11)// PA11
#define M30_Exit_Default	PAout(12)// PA12
#define M30_RX				PAout(8)// PA8

//#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义

#define M30_BOUDRATE	115200	//波特率

#define M30_REBOOT		"at+Rb=1"	//重启
#define M30_CLIENT_IP	"at+CIp=?"	//查询IP地址
#define M30_WIFI_MODE_SOFTAP	"at+WA=1" //softap 网络模式
#define M30_WIFI_MODE_STA		"at+WA=0" //STA 网络模式
#define M30_STA_MODE_AUTO		"at+WM=0" //自动，先手动等待3次,然后Smart connect
#define M30_STA_MODE_SMART		"at+WM=1" //Smart connect 模式
#define M30_STA_MODE_MANUAL		"at+WM=2" //手动

#ifdef USER_SANRISE
	#define AP_SSID		"Sanrise-"  //默认的SSID	
#endif
#ifdef USER_AQUALED
	#define AP_SSID		"AquaLed-"  //默认的SSID
#endif
#ifdef USER_ARCADIA
	#define AP_SSID		"Arcadia-"  //默认的SSID	
#endif
#ifdef USER_SPECTRA
	#define AP_SSID		"Spectra-"  //默认的SSID	
#endif
#ifdef USER_AQUARIUM	
//#define AP_SSID			"Aquarium Systems-"  //默认的SSID
#define AP_SSID			"AQS-"  //默认的SSID
#endif


#define AP_PASSWORD	"12345678"  //默认密码

extern u8 M30_Buf[26];  //保存IP地址

void HLK_M30_IoInit(void);//初始化
u8 HLK_M30_EnterATmode(void);
u8* HLK_M30_GetATCmdAck(u8 *cmd,u16 waittime); //发送命令,返回数据
u8  HLK_M30_SendATCmdAck(u8 *str,u8 *ack,u16 waitime); //发送数据，检查返回结果
void HLK_M30_ExitATmode(void);
void HLK_M30_Reset(void);
void HLK_M30_PowerUp(void);
void HLK_M30_LoadDefault(void);
void Test_HLK_M30(void);
void HLK_M30_SetAPmode(char *ssid,char *password);
void HLK_M30_SetStaSmartCmode(void);
void HLK_M30_ConnectWifiAP(char *ssid,char *password,u8 sam);
u8* HLK_M30_GetIp(void);
u8* HLK_M30_GetMAC(void);
u8* HLK_M30_GetWifiName(void);
void conTowifi(void);

#endif

/*-------- end of file--------*/

