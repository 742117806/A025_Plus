/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: hlk-m30.c
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: HLK-M30ģ�����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "ESP8266_07S.h"
//#include "include.h"
#include <string.h>
#include <ctype.h>
#include <MATH.H>
#include "my_string.h"
#include "usart.h"
#include "delay.h"
#include "rtx_os.h"
#include "CmdProc.h"
#include "menu.h"
#include "func.h"
#define Delay_AT_10ms(X)	OS_delay_10ms(TMR_OTHER,X)

u8 M30_Buf[26];
u8 current_linkID = 0;
u8 wifi_scan_flag = 0;		//����WiFiɨ��ʱ��1������������
u8 wifiConnetFlag = 0;		//1�����ϣ�0�Ͽ�
//���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* HLK_M30_CheckCmd(u8 *str)
{
	char *strx=0;
	if(USART_RECV_OK)		//���յ�һ��������
	{ 
		USART_RX_BUF[USART_RECV_COUNT]='\0';//��ӽ�����
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//����ATָ��2
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8  HLK_M30_SendATCmdAck(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	volatile u16 cntTimes;
	cntTimes = waittime;//*10; 
//	USART_RX_STA=0; //������0
	ResetUartRecv(1); //������0
	if((u32)cmd<=0XFF)
	{
	//	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
	//	USART2->DR=(u32)cmd;
	}
	else 
	{
		printf("%s\r\n",cmd);//��������
	}
	//USART_RX_STA=0; //������0
	
	if(ack&&cntTimes)		//��Ҫ�ȴ�Ӧ��
	{
		while(--cntTimes)	//�ȴ�����ʱ
		{
			//Delay_AT_10ms(1);		//��ʱ����
			delay_ms(3);
			if(USART_RECV_OK)//���յ��ڴ���Ӧ����
			{
			//	USART_RX_BUF[USART_RECV_COUNT]='\0';//��ӽ�����
			//	OLED_ShowString(0,16,(u8*)USART_RX_BUF);
			//	OLED_Refresh_Gram();

				if(HLK_M30_CheckCmd(ack))break;//�õ���Ч���� 
			}
		}
		if(cntTimes==0)res=1; 
	}
	return res;
}

//cmd:���͵��ַ���������Ҫ��ӻس��ˣ�
//ack:�ȴ�Ӧ�����ַ���
//waittime�����͵ȴ���ʱʱ��
//f_number�����ն���֡Ӧ�����ݣ�һ���س�������һ֡����
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8  ESP8266_SendATCmdAck(char *cmd, char *ack, u16 waittime,u8 f_number)
{

	u8 res = 0;
	volatile u16 cntTimes;

	cntTimes = waittime;//*10; 
	SetUartRxType(USART_TYPE_AT);
	ResetUartRecv(f_number); //������0
	printf("%s\r\n", cmd);//��������
	if (ack&&cntTimes)		//��Ҫ�ȴ�Ӧ��
	{
		while (--cntTimes)	//�ȴ�����ʱ
		{
			//delay_ms(10);
			os_dly_wait(1);
			if (HLK_M30_CheckCmd((u8*)ack))
				break;//�õ���Ч���� 
			if ((cntTimes%150) == 0)
				printf("%s\r\n", cmd);//��������
		}
		if (cntTimes == 0)res = 1;
	}
	SetUartRxType(USART_TYPE_LED);
	return res;
}


//ȡATָ��ķ���ֵ
u8* HLK_M30_GetATCmdAck(u8 *cmd,u16 waittime)
{
	u16 cntTimes;
	int post;
	cntTimes = waittime;//*10; 
	ResetUartRecv(2); //������0

	if((u32)cmd<=0XFF)
	{
		//	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		//	USART2->DR=(u32)cmd;
	}
	else 
	{
		printf("%s\r\n",cmd);//��������
	}

	if(cntTimes)		//��Ҫ�ȴ�Ӧ��
	{
		while(--cntTimes)	//�ȴ�����ʱ
		{
			Delay_AT_10ms(1);
			if(USART_RECV_OK)//���յ��ڴ���Ӧ����
			{
				if(HLK_M30_CheckCmd("at+"))break;//�õ���Ч���� 
			}
		}
	}
	//����0˵���յ�����
	if (cntTimes) USART_RX_BUF[USART_RECV_COUNT]='\0';//��ӽ�����
	else USART_RX_BUF[0]='\0';

	post = strpos((char *)USART_RX_BUF,'\n');

	if (post != -1)post +=1;
	else post = 0;

	return (USART_RX_BUF+post);
}

//ȡATָ��ķ���ֵ
u8* ESP8266_GetATCmdAck(u8 *cmd, u8 *ack, u16 waittime)
{
	u16 cntTimes;
	//int post;
	u8 *p;
	
	cntTimes = waittime;//*10;

	ResetUartRecv(0); //������0

	if ((u32)cmd <= 0XFF)
	{
		//	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		//	USART2->DR=(u32)cmd;
	}
	else
	{
		printf("%s\r\n", cmd);//��������
	}

	if (cntTimes)		//��Ҫ�ȴ�Ӧ��
	{
		while (--cntTimes)	//�ȴ�����ʱ
		{
			//Delay_AT_10ms(1);
			delay_ms(10);
			if (USART_RECV_OK)//���յ��ڴ���Ӧ����
			{
				if (HLK_M30_CheckCmd(ack))break;//�õ���Ч���� 
			}
		}
	}
	//����0˵���յ�����
	if (cntTimes) USART_RX_BUF[USART_RECV_COUNT] = '\0';//��ӽ�����
	else USART_RX_BUF[0] = '\0';
	p = (u8*)strstr((char*)USART_RX_BUF, (const char*)ack);
	return p;
}

//����ATָ��ģʽ ,�ɹ�����0��ʧ�ܷ���1
u8 HLK_M30_EnterATmode()
{
	u8 cnt;

	M30_Exit_Default = 0;
	//delay_ms(1000); //0.5~5�����ATģʽ
	Delay_AT_10ms(150);

	M30_Exit_Default = 1;

	//����Ƿ��Ѿ�������ATģʽ
	cnt = 10;
	while(HLK_M30_SendATCmdAck("at","at",200) && cnt--)
	{
		Delay_AT_10ms(1);	
	}
	if (cnt)return 0;
	else return 1;

}

//�˳�ATָ��ģʽ
void HLK_M30_ExitATmode(void)
{
	 HLK_M30_SendATCmdAck("at+TS=1","ok",200);
}

//�����λ
void ESP8266_Reset()
{
	HLK_M30_SendATCmdAck("AT+RST","OK",200);
}

//��λ--Ӳ����ʽ
void ESP8266_HWReset()
{
	M30_reset = 0;
	delay_ms(500);
	M30_reset = 1;
}


//�ָ�Ĭ��
void ESP8266_LoadDefault(void)
{
	HLK_M30_SendATCmdAck("AT+RESTORE","OK",200);		//�ָ��������ã�Ĭ��APģʽ��IP:192.168.4.1,�˿ں�333��
}

//��ȡ��Χ��WiFi��SSID
//void EsSP8266_GetSSID(STRUCT_SYSTEM *cmd)
//{
//	u8 ssid[30] = { 0 };
//	u8 pasword[30] = { 0 };
//	char at_cmd[100];
//	u8 *p = NULL;

//	switch (cmd->type)
//	{
//	case (char)0xEA://��ȡSSID
//		//WriteChar("AT+CWLAP\r\n", strlen("AT+CWLAP\r\n"));
//		ESP8266_SendATCmdAck("AT+CWLAP\r\n", "OK", 200, 2);
//		break;
//	case (char)0xE9://���ü�ͥ����
//		StrObtainStr((u8*)cmd, ssid, '"','"');
//		p = (u8*)strchr((const char*)cmd, ',');
//		StrObtainStr(p, pasword, '"','"');
//		sprintf(at_cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pasword);
//		ESP8266_SendATCmdAck(at_cmd, "OK", 200, 2);//��������·��WiFi����
//		

//		break;
//	default:
//		break;
//	}

//}

void ESP8266_ConnectWifiProc(u8 *recdata)
{
	u8 ssid[30] = { 0 };
	u8 pasword[30] = { 0 };
	char at_cmd[100];
	u8 *p = NULL;

	StrObtainStr(recdata, ssid, '"', '"');
	p = (u8*)strchr((const char*)recdata, ',');
	StrObtainStr(p, pasword, '"', '"');
	sprintf(at_cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pasword);
	os_dly_wait(50);
	ESP8266_SendATCmdAck(at_cmd, "OK", 0, 2);//��������·��WiFi����
	
}

void ESP8266_ScanWifi(void)
{


	u8 i = 0;
	char *p1;
	char *p2;
	char cmd[200] = { 0 };
	char ssid[30] = { 0 };

	//printf("ESP8266_ScanWifi\r\n");
	uart_sendbuf("AT+CWLAPOPT=1,2\r\n", strlen("AT+CWLAPOPT=1,2\r\n"));
	os_dly_wait(30);
	uart_sendbuf("AT+CWLAP\r\n", strlen("AT+CWLAP\r\n"));
	wifi_scan_flag = 1;
	//ESP8266_SendATCmdAck("AT+CWLAPOPT=1,2", "OK", 200, 2);
	//ESP8266_SendATCmdAck("AT+CWLAP", "OK", 0, 2);
	os_dly_wait(300);
	p1 = (char*)TX_RX_BUF;
	cmd[0] = FRAME_START;
	cmd[1] = FRAME_TYPE_LAP;
	cmd[2] = FRAME_MODE_GET;
	strcat(cmd, "::");
	for (i = 0; i < 15; i++)
	{
		p2 = strstr((const char*)p1, "+CWLAP:");
		if ((p2 != NULL))
		{
			StrObtainStr((u8*)p2, (u8*)ssid, '"', '"');
			if ((strlen(cmd) + strlen(ssid)) < 200)
			{
				strcpy(cmd + strlen(cmd), ssid);
				strcat(cmd, "::");
				memset(ssid, 0x00, sizeof(ssid));
				p1 = p2 + 1;
			}
			else
			{
				break;
			}

		}
		else
		{
			break;
		}

	}
	cmd[strlen(cmd)] = FRAME_END;
	//printf("%s", TX_RX_BUF);
	ESP8266_SendDatas(current_linkID, (u8*)cmd, strlen(cmd));
	wifi_scan_flag = 0;
}

/*  APģʽ
at+WA=1
at+Assid=Sanrise-xx
at+Assidl=10
at+Aam=9
at+Apw=12345678
at+Apwl=8
at+dhcp=0
at+Aip=192,168,0,99
at+mask=255,255,255,0
at+gw=192,168,0,1
at+dns=192,168,0,1
at+UType=1
at+UIp=192.168.0.1
at+URPort=0
at+ULPort=8080
at+Ub=115200
at+Ud=8
at+Up=0
at+Us=1
at+Rb=1
*/
//APģʽ
//ssidΪ�գ�ʹ��Ĭ��SANRISE-XXXX
//passwordΪ�գ�ʹ��Ĭ��12345678

void ESP8266_SetAPmode(char *ssid, char *password)
{
#define  BUFCMD_LEN		65
#define  PARA_LEN	20
	extern u8 CPU_GUID[12];
	char bufPara[40] = { 0 };  //�������
	char bufCmd[BUFCMD_LEN] = { 0 };  //����ATָ��
	u8 len1,len2;  //����
	u8 *p;

	
	len1 = strlen(ssid);		//ssid
	len2 = strlen(password);	//����

	//ESP8266_SendATCmdAck("AT+RESTORE", "OK", 1000,2);			//�ָ��������ã�Ĭ��APģʽ��IP:192.168.4.1,�˿ں�333��
	
	//os_dly_wait(200);
	if(sysFlag.Bits.WIFI_MODE == 1)
	{
		ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 300, 2);			//=1:sta,=2:ap,=3:ap+staģʽ
	}
	else
	{
		ESP8266_SendATCmdAck("AT+CWMODE=3", "OK", 300, 2);			//=1:sta,=2:ap,=3:ap+staģʽ
	}
	
	ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//����ģ����ЧSTA����
	os_dly_wait(200);

	if ((len1 > 0) && (len2 >= 8)) //SSID��Ϊ��,���볤��>8
	{
		sprintf(bufCmd, "AT+CWSAP=\"%s\",\"%s\",1,3", ssid,password);	//ssid,����12345678��ͨ��1�����ܷ�ʽ��WPA2_PSK
	}
	else   //SSIDΪ�գ�����Sanrise-xxxx SSID
	{
		//��SSID��MAC��һ������Ĭ�ϡ�12345678��
		p = ESP8266_GetMAC();

		Del_char((char*)p, ':');					//ɾ���ַ����еġ�:��
		LowerCharToUpperChar((char*)p, strlen((const char*)p));	//����Сд���ַ���ɴ�д

		strcat(bufPara, AP_SSID);
		strcat(bufPara, (const char*)p + 8);				//MAC��5λ,MAC��6λ
		sprintf(bufCmd, "AT+CWSAP=\"%s\",\"12345678\",1,3", bufPara);
		strcpy((char*)CPU_GUID,bufPara);
	}
	//delay_ms(1000);
	os_dly_wait(100);
	ESP8266_SendATCmdAck(bufCmd, "OK", 400,2);
	//ESP8266_SendATCmdAck("AT+CIPMUX=1", "OK", 300, 2);			//����������
	uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//����������
	delay_ms(300);
	//ESP8266_SendATCmdAck("AT+CIPSERVER=1,8080", "OK", 300, 2);	//�򿪷����������ö˿ں���8080	
	uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//�򿪷����������ö˿ں���8080	
	//ChangeMenu(KEY_EXIT);
}



/*  STA Smart Cģʽ
at+WM=0
at+WA=0
at+Sssid=CTC-2538
at+Sssidl=8
at+Sam=9
at+Spw=12345678
at+Spwl=8
at+dhcp=1
at+ip=192,168,0,99
at+mask=255,255,255,0
at+gw=192,168,0,1
at+dns=192,168,0,1
at+UType=1
at+UIp=192.168.0.1
at+URPort=0
at+ULPort=8080
at+Ub=115200
at+Ud=8
at+Up=0
at+Us=1
at+Rb=1
*/
//APģʽ
void HLK_M30_SetStaSmartCmode() 
{
	HLK_M30_EnterATmode();

	HLK_M30_SendATCmdAck("at+Df=1","ok",200); //�ָ�Ĭ��	
	HLK_M30_SendATCmdAck("at+WM=0","ok",200);  //Smart connect
	HLK_M30_SendATCmdAck("at+WA=0","ok",200);
	HLK_M30_SendATCmdAck("at+Sssid=","ok",200);
	HLK_M30_SendATCmdAck("at+Sssidl=0","ok",200);
	HLK_M30_SendATCmdAck("at+Sam=7","ok",200);
	HLK_M30_SendATCmdAck("at+Spw=","ok",200);
	HLK_M30_SendATCmdAck("at+Spwl=0","ok",200);
	HLK_M30_SendATCmdAck("at+dhcp=0","ok",200);
	HLK_M30_SendATCmdAck("at+ip=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+mask=255,255,255,0","ok",200);
	HLK_M30_SendATCmdAck("at+gw=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+dns=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+UType=1","ok",200);
	HLK_M30_SendATCmdAck("at+UIp=192.168.0.1","ok",200);
	HLK_M30_SendATCmdAck("at+URPort=0","ok",200);
	HLK_M30_SendATCmdAck("at+ULPort=8080","ok",200);
	HLK_M30_SendATCmdAck("at+Ub=115200","ok",200);
	HLK_M30_SendATCmdAck("at+Ud=8","ok",200);
	HLK_M30_SendATCmdAck("at+Up=0","ok",200);
	HLK_M30_SendATCmdAck("at+Us=1","ok",200);
	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);

//	OLED_ShowString(0,0,"Set STA C mode");
//	OLED_Refresh_Gram();
}

//����ģ��SmartConfig
void ESP8266_StaSmartConfig(void)
{
		
	ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 200, 2);//STAģʽ	
	ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//����ģ����ЧSTA����
	delay_ms(3000);
	ESP8266_SendATCmdAck("AT+CWSTARTSMART", "OK", 200, 2);//��ʼSmartConfig
	//AT + CWSTOPSMART //ֹͣSmartConfig
}

//ģ��ͨ��ATָ������WiFi
void ESP8266_ConnecetWifi(char *ssid, char *password)
{
	char bufCmd[65] = { 0 };  //����ATָ��

	if ((ssid > 0) && (password > 0))
	{
		sprintf(bufCmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
		//ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 200, 2);//STAģʽ	
		//uart_sendbuf("AT+CWMODE=1\r\n",0);//STAģʽ	
		//ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//����ģ����ЧSTA����
		//uart_sendbuf("AT+RST\r\n",0);//����ģ����ЧSTA����
		delay_ms(3000);
		//ESP8266_SendATCmdAck(bufCmd, "OK", 200, 2);
		uart_sendbuf((u8*)bufCmd,0);
		delay_ms(3000);
		//ESP8266_SendATCmdAck("AT+CIPMUX=1", "OK", 300, 2);			//����������
		uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//����������
		delay_ms(300);
		//ESP8266_SendATCmdAck("AT+CIPSERVER=1,8080", "OK", 300, 2);	//�򿪷����������ö˿ں���8080	
		uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//�򿪷����������ö˿ں���8080	
	}
}

/*----------------------------------------*/
/*
at+WM=2
at+WA=0
at+Sssid=CTC-2538
at+Sssidl=8
at+Sam=9
at+Spw=12345678
at+Spwl=8
at+dhcp=1
at+ip=192,168,0,99
at+mask=255,255,255,0
at+gw=192,168,0,1
at+dns=192,168,0,1
at+UType=1
at+UIp=192.168.0.1
at+URPort=0
at+ULPort=8080
at+Ub=115200
at+Ud=8
at+Up=0
at+Us=1
at+WC=1
at+Rb=1

*/

//����WIFI AP
//void HLK_M30_ConnectWifiAP(char *ssid,char *password,u8 sam) 
//{
//#define  BUFCMD_LEN		65
//	char bufCmd[BUFCMD_LEN]={0};  //����ATָ��
//	u8 lenSsid;  //SSID����
//	u8 lenPassword; //���볤��
//
//	if(HLK_M30_EnterATmode())return; //���ܽ���ATģʽ�ͷ���
//
//	lenSsid = strlen(ssid);
//	lenPassword = strlen(password);
//
//	if (lenSsid==0 || lenPassword < 8) //���Ȳ���Ϊ0
//	{
//		return;
//	}
//	
//	HLK_M30_SendATCmdAck("at+WM=2","ok",200);
//	HLK_M30_SendATCmdAck("at+WA=0","ok",200);
//
//	//ssidָ��
//	strcat(bufCmd,"at+Sssid=");
//	strcat(bufCmd,ssid);
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //����SSID����
//
//	//sssidl
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //����ڴ�
//	strcat(bufCmd,"at+Sssidl=");
//	strcat(bufCmd,itostr(lenSsid));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SSID����
//
//	//��������
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //����ڴ�
//	strcat(bufCmd,"at+Sam=");
//	strcat(bufCmd,itostr(sam));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SAM��������
//
//	//����
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //����ڴ�
//	strcat(bufCmd,"at+Spw=");
//	strcat(bufCmd,password);
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200); //��������
//
//	//���볤��
//	ClrMem((void*)bufCmd,BUFCMD_LEN);
//	strcat(bufCmd,"at+Spwl=");
//	strcat(bufCmd,itostr(lenPassword));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //���볤��
//
//	HLK_M30_SendATCmdAck("at+dhcp=1","ok",200);
//	HLK_M30_SendATCmdAck("at+ip=192,168,0,99","ok",200);
//	HLK_M30_SendATCmdAck("at+mask=255,255,255,0","ok",200);
//	HLK_M30_SendATCmdAck("at+gw=192,168,0,1","ok",200);
//	HLK_M30_SendATCmdAck("at+dns=192,168,0,1","ok",200);
//	HLK_M30_SendATCmdAck("at+UType=1","ok",200);
//	HLK_M30_SendATCmdAck("at+UIp=192.168.0.1","ok",200);
//	HLK_M30_SendATCmdAck("at+URPort=0","ok",200);
//	HLK_M30_SendATCmdAck("at+ULPort=8080","ok",200);
//	HLK_M30_SendATCmdAck("at+Ub=115200","ok",200);
//	HLK_M30_SendATCmdAck("at+Ud=8","ok",200);
//	HLK_M30_SendATCmdAck("at+Up=0","ok",200);
//	HLK_M30_SendATCmdAck("at+Us=1","ok",200);
//	HLK_M30_SendATCmdAck("at+WC=1","ok",800);//WIFIģ�����PMKʱ��ϳ� 
//
//	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
//}

void conTowifi(void)
{
	if(HLK_M30_EnterATmode())return; //���ܽ���ATģʽ�ͷ���
	
	HLK_M30_SendATCmdAck("at+WM=2","ok",200);
	HLK_M30_SendATCmdAck("at+WA=0","ok",200);
	HLK_M30_SendATCmdAck("at+Sssid=H60-L19","ok",200);
	HLK_M30_SendATCmdAck("at+Sssidl=7","ok",200);
	HLK_M30_SendATCmdAck("at+Sam=9","ok",200);
	HLK_M30_SendATCmdAck("at+Spw=12345678","ok",200);
	HLK_M30_SendATCmdAck("at+Spwl=8","ok",200);
	HLK_M30_SendATCmdAck("at+dhcp=1","ok",200);
	HLK_M30_SendATCmdAck("at+ip=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+mask=255,255,255,0","ok",200);
	HLK_M30_SendATCmdAck("at+gw=192,168,0,1","ok",200);
	HLK_M30_SendATCmdAck("at+dns=192,168,0,1","ok",200);
	HLK_M30_SendATCmdAck("at+UType=1","ok",200);
	HLK_M30_SendATCmdAck("at+UIp=192.168.0.1","ok",200);
	HLK_M30_SendATCmdAck("at+URPort=0","ok",200);
	HLK_M30_SendATCmdAck("at+ULPort=8080","ok",200);
	HLK_M30_SendATCmdAck("at+Ub=115200","ok",200);
	HLK_M30_SendATCmdAck("at+Ud=8","ok",200);
	HLK_M30_SendATCmdAck("at+Up=0","ok",200);
	HLK_M30_SendATCmdAck("at+Us=1","ok",200);
	HLK_M30_SendATCmdAck("at+WC=1","ok",2800);
	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
}

//��ȡ����M30_Buf
u8* HLK_M30_GetIp(void)
{
	u8 *p;
	int star,end,i,j;

	if (HLK_M30_SendATCmdAck("at+CIp=?","CIp",200)==0)
	{
		USART_RX_BUF[USART_RECV_COUNT]='\0';//��ӽ�����
		star = strpos((char *)USART_RX_BUF,'\n');
		end = strpos((char *)USART_RX_BUF,',');
		if ((star != -1) && (end != -1) && end > star)
		{
			p = USART_RX_BUF+star+1;
			j = end - star -2;
			for(i=0;i<=j;i++)
			{
				M30_Buf[i] = *p++;
			}
			M30_Buf[i]='\0';
		}	
	}
	else
	{
			M30_Buf[0]='\0';
	}
	return M30_Buf;
}

//��ȡ����ģ��IP
u8* ESP8266_GetIp(void)
{
//	u8 *p;
//	int star, end, i, j;

	if (ESP8266_SendATCmdAck("AT+CIFSR", "+CIFSR", 200,1) == 0)
	{
		USART_RX_BUF[USART_RECV_COUNT] = '\0';//��ӽ�����
		StrObtainStr(USART_RX_BUF, M30_Buf,'"','"');
	}
	else
	{
		M30_Buf[0] = '\0';
	}
	return M30_Buf;
}

//��ȡWIFI���ƻ����ӵ�AP����
//u8* HLK_M30_GetWifiName(void)
//{
//	u8 *p;
//
//	p = HLK_M30_GetATCmdAck("at+WA=?",200); //ȡ��WIFI����ģʽ
//
//	if (*p == '1') //APģʽ
//	{
//		p = HLK_M30_GetATCmdAck("at+Assid=?",200);
//	} 
//	else  //����ģʽ
//	{
//		p = HLK_M30_GetATCmdAck("at+Sssid=?",200);
//	}
//
//	strcpy((char*)&M30_Buf,(char*)p);
//	return M30_Buf;
//}

//��ȡWIFI���ƻ����ӵ�AP����
u8* ESP8266_GetWifiName(void)
{
//	u8 *p;

	if (ESP8266_SendATCmdAck("AT+CWJAP?", "+CWJAP:", 200, 1) == 0) //ȡ��WIFI�������Ϣ
	{
		StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
	}
	else
	{
		M30_Buf[0] = '\0';
	}
	return M30_Buf;
}

//��ѯ��ǰWiFiģ��Ĺ���ģʽ
char ESP8266_GetWifiMode(void)
{
	char mode = 0;
	const char *p;
	if (ESP8266_SendATCmdAck("AT+CWMODE?", "+CWMODE:", 200, 2) == 0)
	{
		p = StrStr((const char*)USART_RX_BUF, "+CWMODE:");
		if (p > 0)
		{
			mode = *(p + strlen("+CWMODE:"));
		}
	}
	return mode;
}
//��ȡMAC��ַ
//����ָ��MAC��ַ�����ָ��[] 6���ֽ�
u8* ESP8266_GetMAC(void)
{
//	u8 *p;
//	u8 strMac[4];
//	u8 i,j;
	char wifi_mode;

	wifi_mode = ESP8266_GetWifiMode();

	if (wifi_mode == '1')			//STAģʽ
	{
		if (ESP8266_SendATCmdAck("AT+CIPSTAMAC?", "OK", 200, 3) == 0)
		{
			ClrMem((void*)M30_Buf, 6);
			StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
		}
	}
	else if ((wifi_mode == '2') || (wifi_mode == '3'))	//wifiģʽ+STA
	{
		if (ESP8266_SendATCmdAck("AT+CIPAPMAC?", "OK", 200, 3) == 0)
		{
			ClrMem((void*)M30_Buf, 6);
			StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
		}
	}
	else 		//����ģʽ
	{

	}

	//M30_Buf[6]='\0'; //MAC��ַֻ��6λ
	return M30_Buf;
}

//IO�ڳ�ʼ��
void HLK_M30_IoInit(void)
{

	//GPIO_InitTypeDef  GPIO_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//pin8:�ϵ縴λ��;pin11:RST_N;pin12:ES_RST,
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	//GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	//GPIO_SetBits(GPIOA,GPIO_Pin_11|);				 //PB.5 �����

}

//ESP8266 WiFiģ���ʼ��
void ESP8266_Init(void)
{
	uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//����������
	delay_ms(300);
	uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//�򿪷����������ö˿ں���8080	
	ESP8266_SetAPmode("", "");					
	
	//ESP8266_ConnecetWifi("CMCC-FQH","18823831312");
}

//ESP8266��������
void ESP8266_SendDatas(u8 linkID, u8* datas, u16 lenght)
{
	u8 temp[50] = { 0 };

	sprintf((char*)temp, "AT+CIPSEND=%d,%d\r\n", linkID, lenght);
	//ESP8266_SendATCmdAck((char*)temp, "OK", 200,2);
	//ESP8266_SendATCmdAck((char*)temp, 0, 200, 2);
	uart_sendbuf(temp, strlen((const char*)temp));
	delay_ms(10);
	uart_sendbuf(datas, lenght);
}

void ESP8266_SendDatasToAll(u8 *datas, u8 lenght)
{
	u8 i;
	u8 temp[200] = { 0 };

	for (i = 0; i < 5;i++)		//֧��5���û�
	{
		sprintf((char*)temp, "AT+CIPSEND=%d,%d", i, lenght);
		if (ESP8266_SendATCmdAck((char*)temp, ">", 100, 3) == 0)
		{
			uart_sendbuf(datas, lenght);
		}
		
	}
}
/*
void Test_HLK_M30()
{
	u8 i;
	HLK_M30_EnterATmode();

	while(HLK_M30_SendATCmdAck("at","at",200))
	{
		
		delay_ms(50);
		OLED_Clear_Ram();
	//	OLED_ShowChar(50,0,'Y',FONT12,0);
		OLED_ShowNum(30,0,i++,3,FONT12);
		OLED_ShowString(0,0,"NG");
		OLED_Refresh_Gram();
		HLK_M30_EnterATmode();
	}


	if (HLK_M30_SendATCmdAck("at+GW=0,0","ok",200)==0)
	{
		OLED_ShowChar(50,0,'Y',FONT12,0);
		OLED_ShowString(0,16,(u8*)USART_RX_BUF);
		OLED_Refresh_Gram();
	}
	else
	{
		OLED_ShowChar(50,0,'N',FONT12,0);
		OLED_ShowString(20,16,(u8*)USART_RX_BUF);
		OLED_Refresh_Gram();
	}
	
	HLK_M30_GetIp();
	//delay_ms(1000);
	//HLK_M30_GetMAC();
	delay_ms(1000);

	//HLK_M30_SetAPmode("","");

	OLED_ShowString(0,16,"Set ap finish.");
	OLED_Refresh_Gram();

	HLK_M30_ExitATmode();

}
*/

/*-------- end of file--------*/
