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

#ifndef __ESP8266_O7S_H__
#define __ESP8266_O7S_H__	 
#include "sys.h"
#include "system.h"
#include "CmdProc.h"
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

typedef enum 
{
	MAC_AP,
	MAC_STA,
}mac_type_e;

extern u8 M30_Buf[26];  //保存IP地址
extern u8 current_linkID;	//当前连接的客户端ID（0-4）
extern u8 wifi_scan_flag;
extern u8 wifiConnetFlag;


void HLK_M30_IoInit(void);//初始化
void ESP8266_Init(void);
void ESP8266_SendDatas(u8 linkID, u8* datas, u16 length);
void ESP8266_SendDatasToAll(u8 *datas, u8 lenght);
u8 HLK_M30_EnterATmode(void);
u8* HLK_M30_GetATCmdAck(u8 *cmd,u16 waittime); //发送命令,返回数据
u8* ESP8266_GetATCmdAck(u8 *cmd, u8 *ack, u16 waittime);
u8  HLK_M30_SendATCmdAck(u8 *str, u8 *ack, u16 waitime); //发送数据，检查返回结果
u8 ESP8266_SendATCmdAck(char *cmd, char *ack, u16 waittime, u8 f_number);
void HLK_M30_ExitATmode(void);
void ESP8266_Reset(void);
void ESP8266_LoadDefault(void);
void EsSP8266_GetSSID(STRUCT_SYSTEM *cmd);
void ESP8266_ConnectWifiProc(u8 *recdata);
void ESP8266_ScanWifi(void);
void Test_HLK_M30(void);
void ESP8266_SetAPmode(char *ssid,char *password);
void HLK_M30_SetStaSmartCmode(void);
void ESP8266_StaSmartConfig(void);
void ESP8266_ConnecetWifi(char *ssid, char *password);
//void HLK_M30_ConnectWifiAP(char *ssid, char *password, u8 sam);
u8* HLK_M30_GetIp(void);
u8* ESP8266_GetIp(void);
u8* ESP8266_GetMAC(void);
//u8* HLK_M30_GetWifiName(void);
u8* ESP8266_GetWifiName(void);
char ESP8266_GetWifiMode(void);
void conTowifi(void);
#endif

/*-------- end of file--------*/

