/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: LEDָ�����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef H_CMD_PROC_H
#define H_CMD_PROC_H

#include "sys.h"

//#define	FRAME_START		241	//0xF1 ֡��ʼ
#define	FRAME_START		242	//0xF2 ֡��ʼ
#define	FRAME_END		243	//0xF3 ֡����
#define	FRAME_MODE_SET	250	//0xFA ������
#define	FRAME_MODE_GET	252	//0xFC ȡ����

//����OK
#ifndef RET_OK
#define RET_OK		245		//0xF5
#endif

//����NG
#ifndef RET_NG
#define RET_NG		246		//0xF6
#endif


#define	FRAME_TYPE_SYSTEM	225	//0xE1 ϵͳָ��
#define FRAME_TYPE_DIMMER	226	//0xE2 ����
#define	FRAME_TYPE_TIME		227	//0xE3 ʱ��
#define	FRAME_TYPE_TIMER	228	//0xE4 ��ʱ
#define	FRAME_TYPE_WEATHER	229	//0xE5 ����
#define	FRAME_TYPE_OTHTER	232	//0xE8 ����ָ��
#define	FRAME_TYPE_JAP		233	//0xE9 ���ü�ͥ����
#define	FRAME_TYPE_LAP		234	//0xEA ��ȡWIFI�б�
#define FRAME_TYPE_MOTOR	235	//0xEB �������

#define	FRAME_CMD_CLOUD		209	//0xD1 �Ʋ�
#define	FRAME_CMD_LIGHTNING	210	//0xD2 ����

#define	FRAME_CMD_DEFAULT	211	//0xD3 Ĭ������
//#define	FRAME_CMD_POWER		212	//0xD4 LED����
#define	FRAME_CMD_FIRMWARE	213	//0xD5 �����̼�
#define	FRAME_CMD_TEMPERATURE	214 //0xD6 �¶�
#define	FRAME_CMD_FAN		215 //0xD7 ����ת��
#define	FRAME_CMD_VERSION	216 //0xD8 �̼��汾
#define	FRAME_CMD_EXIST 	217	//0xD9	����ȷ��
#define	FRAME_CMD_CUSTOM	218	//0xDA	//�ͻ���ʶ����
#define	FRAME_CMD_WORKMODE	219	//0xDB	//����ģʽ
#define	FRAME_CMD_PWM_NUMBER	220	//0xDC	//PWM����
#define	FRAME_CMD_LED_STA		221	//0xDD	//ָʾ��״̬
#define FRAME_CMD_WIFI		222 //0xDE //����WIFI

//ָ��ȹ̶�Ϊ20���ֽ�

//ʱ��ṹ��
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 nc[10];  //����,δʹ��
	u8 end;
}STRUCT_TIME; 

//��ʱ�ṹ��
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 index;	//��ʱ���
	u8 enable;
	u8 hour;
	u8 minute;
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc[5];  //����,δʹ��
	u8 count; //��ʱ����
	u8 end;
}STRUCT_TIMER;

//�����ṹ��
typedef struct {
	u8 start;	//
	u8 type;	//
	u8 mode;
	u8 effect;	//��Ч����
	u8 enable;	//
	u8 index;	//���:1~4, ���Ϊ0,��ִ��Ԥ��
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //ǿ���ȼ�
	u8 nc[7];	//����,δʹ��
	u8 count;  //��Ч����
	u8 end;	//
}STRUCT_WEATHER;

//��������ṹ��
typedef struct {
	u8 start;	//
	u8 type;	//
	u8 mode;
	u8 effect;	//��Ч����
	u8 enable;	//
	u8 day;		//�������ڣ�1-28�ţ�
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //ǿ���ȼ�
	u8 nc;	//����,δʹ��
	u8 end;	//
}STRUCT_MOONLIGH;

//����ṹ��
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc[10];
	u8 end;
}STRUCT_DIMMER;

//ϵͳ�ṹ��
typedef struct {
	u8 start;
	u8 type;	//ϵͳ
	u8 mode;	//����:GET\SET
	u8 command;	
	u8 para[15];	//15������: ����ģʽ(1)������ת��(1)���¶�(1)������汾(2)���ͻ�����(2)������·��(1)��
	u8 end;
}STRUCT_SYSTEM;


//��������������ýṹ��
typedef struct
{
	u8 frame_h;	//֡ͷ
	u8 frame_cmd;	//��������û��߻�ȡʲô��Ϣ��ʶ���ֽڣ�
	u8 frame_mode;	//�����APP��˵�����������ݣ�FA�����ǻ�ȡ���ݣ�FC��
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
	u8 frame_end;
}STRUCT_MOTOR;

#define SYSTEM_MODE		0  //����ģʽ
#define SYSTEM_FANSPEED	1	//����ת��
#define SYSTEM_TEMP		2  //�¶�
#define SYSTEM_VERSION1	3  //����汾1
#define SYSTEM_VERSION2	4  //����汾2
#define SYSTEM_CUSTOM_CODE1		5  //�ͻ�����1
#define SYSTEM_CUSTOM_CODE2		6  //�ͻ�����2
#define SYSTEM_PWM_NUM	7		//����·��
#define SYSTEM_TIMER_INDEX	8		//��ǰ��ʱ��
#define SYSTEM_LED_STATE	9		//��ǰ��ʱ��

#define SYSTEM_GUID0	11
#define SYSTEM_GUID1	12
#define SYSTEM_GUID2	13
#define SYSTEM_EXIST	14  //����

#define TX_RX_BUF	USART_RX_BUF //����/���ͻ���

#define  pTime 			((STRUCT_TIME *)TX_RX_BUF) //ʱ��	
#define  pTimer 		((STRUCT_TIMER *)TX_RX_BUF) //��ʱ
#define  pWeather 		((STRUCT_WEATHER *)TX_RX_BUF) //����
#define  pSystem		((STRUCT_SYSTEM *)TX_RX_BUF) //ϵͳ
#define  pDimmer		((STRUCT_DIMMER *)TX_RX_BUF) //����
#define  PMoonLight		((STRUCT_MOONLIGH *)TX_RX_BUF)//����
#define  pMotor			((STRUCT_MOTOR *)TX_RX_BUF)//���	

void SetMoonlingProc(void);
void LedCmdProc(void);
void ClearRam(u8 *addr,u16 len);
#endif

