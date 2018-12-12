/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: hlk-m30.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: HLK-M30ģ�����
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

//#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//�ߵ��ֽڽ����궨��

#define M30_BOUDRATE	115200	//������

#define M30_REBOOT		"at+Rb=1"	//����
#define M30_CLIENT_IP	"at+CIp=?"	//��ѯIP��ַ
#define M30_WIFI_MODE_SOFTAP	"at+WA=1" //softap ����ģʽ
#define M30_WIFI_MODE_STA		"at+WA=0" //STA ����ģʽ
#define M30_STA_MODE_AUTO		"at+WM=0" //�Զ������ֶ��ȴ�3��,Ȼ��Smart connect
#define M30_STA_MODE_SMART		"at+WM=1" //Smart connect ģʽ
#define M30_STA_MODE_MANUAL		"at+WM=2" //�ֶ�

#ifdef USER_SANRISE
	#define AP_SSID		"Sanrise-"  //Ĭ�ϵ�SSID	
#endif
#ifdef USER_AQUALED
	#define AP_SSID		"AquaLed-"  //Ĭ�ϵ�SSID
#endif
#ifdef USER_ARCADIA
	#define AP_SSID		"Arcadia-"  //Ĭ�ϵ�SSID	
#endif
#ifdef USER_SPECTRA
	#define AP_SSID		"Spectra-"  //Ĭ�ϵ�SSID	
#endif
#ifdef USER_AQUARIUM	
//#define AP_SSID			"Aquarium Systems-"  //Ĭ�ϵ�SSID
#define AP_SSID			"AQS-"  //Ĭ�ϵ�SSID
#endif


#define AP_PASSWORD	"12345678"  //Ĭ������

typedef enum 
{
	MAC_AP,
	MAC_STA,
}mac_type_e;

extern u8 M30_Buf[26];  //����IP��ַ
extern u8 current_linkID;	//��ǰ���ӵĿͻ���ID��0-4��
extern u8 wifi_scan_flag;
extern u8 wifiConnetFlag;


void HLK_M30_IoInit(void);//��ʼ��
void ESP8266_Init(void);
void ESP8266_SendDatas(u8 linkID, u8* datas, u16 length);
void ESP8266_SendDatasToAll(u8 *datas, u8 lenght);
u8 HLK_M30_EnterATmode(void);
u8* HLK_M30_GetATCmdAck(u8 *cmd,u16 waittime); //��������,��������
u8* ESP8266_GetATCmdAck(u8 *cmd, u8 *ack, u16 waittime);
u8  HLK_M30_SendATCmdAck(u8 *str, u8 *ack, u16 waitime); //�������ݣ���鷵�ؽ��
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

