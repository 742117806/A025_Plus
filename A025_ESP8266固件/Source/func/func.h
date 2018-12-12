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
void Language(void);//�����л�
void saveLanguage(void);
void PilotLight(void); //LEDָʾ��

void CheckUdataKey(void); //������������

void ShowUpdataProgress(void);//��������

void SetPreveiwTime(u8 hour,u8 minute);

void SystemLoadDefault(void);//ϵͳĬ������
void AdjFanByTmp(void);

void ConfigWifiStaConn(void); //����STAģʽ����
//-----------------------------------
void SetCustomCode(u16 code1);//���ÿͻ�����
u16 GetCustomCode(void);//��ȡ�ͻ�����

void SetPwmNumber(u8 num);//���õ���·��
u8 GetPwmNumber(void);//��ȡ����·��

u8 GetGUID(u8 index);//��ȡGUID��index = 0~11
void ReadGUID(void); //��ȡGUID��������

void LedIndicatorFlash(void); //ָʾ����˸
void BeepSong(void);//����������

void ErasureUserLogo(void);
void DisplayUserLogo(void);

extern void(*ClockFunc)(void);
#define SetClockFun(X)	ClockFunc=X //����ÿ��Ҫִ�еĺ���

#define START_TMP	38  //���������¶�
#define MAX_TMP		46  //�����¶� 
#define OVER_TMP	68  //���±���

//ϵͳ��Ϣ�ṹ��
typedef struct _SysInfo{
	u16 ledIndicator;    //LEDָʾ����˸��ʽ
	u8 workmode;    //����ģʽ
	u8 indexTimer;  //��ǰ��ʱ��
	u8 nextTimer;   //��һ����ʱ��
	u8 pwmNumber;   //����·��
	u8 pwmSpeed;	//PWM�仯���ٶ�
	u8 language;	//���� 0:Ӣ��  1:����
	u8 updataType;  //��������
	u8 temperature;	//�ƾ��¶�
	u8 OverProtect;  //���±���
	u8 MunualFan;	//�ֶ�����FAN
	u8 Beep_type; //������������ 
}SYSINFO;


//��������Ϣ
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

//������ λ��־
typedef union _flag{
	u16 bytes;
	struct _b{
		u16 LED_sta		:1;  //����ָʾ��
	//	u16 LED_wifi		:1;  //WIFIָʾ��
		u16 LED_SAVE	:1;  //����ָʾ��״̬
		uint16_t WIFI_MODE:2;	//WiFi����ģʽ1STA��2AP��3AP+STA
	}Bits;
}SYSFLAG;

extern SYSINFO sysInfo;
extern SYSFLAG sysFlag;
extern MotorInfo_s motorInfo;		//��������Ϣ
//extern uint8_t  connectingWiFiFlag;	//��������WiFi·����ָʾ��
#define SetWorkMode(X)	sysInfo.workmode=X
#define SetLedIndicator(X)  sysInfo.ledIndicator=X  //����ָʾ����˸��ʽ
#define SetPwmSpeed(X)	sysInfo.pwmSpeed=X	//����PWM�仯�ٶ�

#define SetBeepType(X) sysInfo.Beep_type=X //���÷�����

#define ENGELISH	0  //Ӣ��
#define CHINESE		1  //����

#define IF_EN()  if(sysInfo.language==ENGELISH)
#define IF_CN()  if(sysInfo.language==CHINESE)

//LEDָʾ����˸��ʽ
#define LED_ITR_OFF  0x0000	//�ر�
#define LED_ITR_ONE	 0x0F0F	//1����һ��
#define LED_ITR_TWO  0x0009	//������,��1��
#define LED_ITR_THREE   0x0049	//������,��һ��
#define LED_ITR_FLASH	0x5555  //����
#define LED_ITR_ON	 0xFFFF	//һֱ��

//��������
#define BEEP_OFF  0x00 //�ر�
#define BEEP_ONE  0x80 //��һ��
#define BEEP_TWO  0xA0 //������
#define BEEP_THREE	0xA8 //������

void ShowTwoNum(u8 x,u8 y,u8 num,u8 mode);

#endif
