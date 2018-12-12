/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: hlk-m30.c
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: HLK-M30模块操作
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
u8 wifi_scan_flag = 0;		//进行WiFi扫描时置1，结束后清零
u8 wifiConnetFlag = 0;		//1连接上，0断开
//发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* HLK_M30_CheckCmd(u8 *str)
{
	char *strx=0;
	if(USART_RECV_OK)		//接收到一次数据了
	{ 
		USART_RX_BUF[USART_RECV_COUNT]='\0';//添加结束符
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//发送AT指令2
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8  HLK_M30_SendATCmdAck(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	volatile u16 cntTimes;
	cntTimes = waittime;//*10; 
//	USART_RX_STA=0; //接收清0
	ResetUartRecv(1); //接收清0
	if((u32)cmd<=0XFF)
	{
	//	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
	//	USART2->DR=(u32)cmd;
	}
	else 
	{
		printf("%s\r\n",cmd);//发送命令
	}
	//USART_RX_STA=0; //接收清0
	
	if(ack&&cntTimes)		//需要等待应答
	{
		while(--cntTimes)	//等待倒计时
		{
			//Delay_AT_10ms(1);		//暂时屏蔽
			delay_ms(3);
			if(USART_RECV_OK)//接收到期待的应答结果
			{
			//	USART_RX_BUF[USART_RECV_COUNT]='\0';//添加结束符
			//	OLED_ShowString(0,16,(u8*)USART_RX_BUF);
			//	OLED_Refresh_Gram();

				if(HLK_M30_CheckCmd(ack))break;//得到有效数据 
			}
		}
		if(cntTimes==0)res=1; 
	}
	return res;
}

//cmd:发送的字符串，不需要添加回车了，
//ack:等待应答是字符串
//waittime：发送等待超时时间
//f_number：接收多少帧应答数据，一个回车换行算一帧数据
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8  ESP8266_SendATCmdAck(char *cmd, char *ack, u16 waittime,u8 f_number)
{

	u8 res = 0;
	volatile u16 cntTimes;

	cntTimes = waittime;//*10; 
	SetUartRxType(USART_TYPE_AT);
	ResetUartRecv(f_number); //接收清0
	printf("%s\r\n", cmd);//发送命令
	if (ack&&cntTimes)		//需要等待应答
	{
		while (--cntTimes)	//等待倒计时
		{
			//delay_ms(10);
			os_dly_wait(1);
			if (HLK_M30_CheckCmd((u8*)ack))
				break;//得到有效数据 
			if ((cntTimes%150) == 0)
				printf("%s\r\n", cmd);//发送命令
		}
		if (cntTimes == 0)res = 1;
	}
	SetUartRxType(USART_TYPE_LED);
	return res;
}


//取AT指令的返回值
u8* HLK_M30_GetATCmdAck(u8 *cmd,u16 waittime)
{
	u16 cntTimes;
	int post;
	cntTimes = waittime;//*10; 
	ResetUartRecv(2); //接收清0

	if((u32)cmd<=0XFF)
	{
		//	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		//	USART2->DR=(u32)cmd;
	}
	else 
	{
		printf("%s\r\n",cmd);//发送命令
	}

	if(cntTimes)		//需要等待应答
	{
		while(--cntTimes)	//等待倒计时
		{
			Delay_AT_10ms(1);
			if(USART_RECV_OK)//接收到期待的应答结果
			{
				if(HLK_M30_CheckCmd("at+"))break;//得到有效数据 
			}
		}
	}
	//不是0说明收到数据
	if (cntTimes) USART_RX_BUF[USART_RECV_COUNT]='\0';//添加结束符
	else USART_RX_BUF[0]='\0';

	post = strpos((char *)USART_RX_BUF,'\n');

	if (post != -1)post +=1;
	else post = 0;

	return (USART_RX_BUF+post);
}

//取AT指令的返回值
u8* ESP8266_GetATCmdAck(u8 *cmd, u8 *ack, u16 waittime)
{
	u16 cntTimes;
	//int post;
	u8 *p;
	
	cntTimes = waittime;//*10;

	ResetUartRecv(0); //接收清0

	if ((u32)cmd <= 0XFF)
	{
		//	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		//	USART2->DR=(u32)cmd;
	}
	else
	{
		printf("%s\r\n", cmd);//发送命令
	}

	if (cntTimes)		//需要等待应答
	{
		while (--cntTimes)	//等待倒计时
		{
			//Delay_AT_10ms(1);
			delay_ms(10);
			if (USART_RECV_OK)//接收到期待的应答结果
			{
				if (HLK_M30_CheckCmd(ack))break;//得到有效数据 
			}
		}
	}
	//不是0说明收到数据
	if (cntTimes) USART_RX_BUF[USART_RECV_COUNT] = '\0';//添加结束符
	else USART_RX_BUF[0] = '\0';
	p = (u8*)strstr((char*)USART_RX_BUF, (const char*)ack);
	return p;
}

//进入AT指令模式 ,成功返回0，失败返回1
u8 HLK_M30_EnterATmode()
{
	u8 cnt;

	M30_Exit_Default = 0;
	//delay_ms(1000); //0.5~5秒进入AT模式
	Delay_AT_10ms(150);

	M30_Exit_Default = 1;

	//检查是否已经运行在AT模式
	cnt = 10;
	while(HLK_M30_SendATCmdAck("at","at",200) && cnt--)
	{
		Delay_AT_10ms(1);	
	}
	if (cnt)return 0;
	else return 1;

}

//退出AT指令模式
void HLK_M30_ExitATmode(void)
{
	 HLK_M30_SendATCmdAck("at+TS=1","ok",200);
}

//软件复位
void ESP8266_Reset()
{
	HLK_M30_SendATCmdAck("AT+RST","OK",200);
}

//复位--硬件方式
void ESP8266_HWReset()
{
	M30_reset = 0;
	delay_ms(500);
	M30_reset = 1;
}


//恢复默认
void ESP8266_LoadDefault(void)
{
	HLK_M30_SendATCmdAck("AT+RESTORE","OK",200);		//恢复出厂设置（默认AP模式，IP:192.168.4.1,端口号333）
}

//获取周围的WiFi的SSID
//void EsSP8266_GetSSID(STRUCT_SYSTEM *cmd)
//{
//	u8 ssid[30] = { 0 };
//	u8 pasword[30] = { 0 };
//	char at_cmd[100];
//	u8 *p = NULL;

//	switch (cmd->type)
//	{
//	case (char)0xEA://获取SSID
//		//WriteChar("AT+CWLAP\r\n", strlen("AT+CWLAP\r\n"));
//		ESP8266_SendATCmdAck("AT+CWLAP\r\n", "OK", 200, 2);
//		break;
//	case (char)0xE9://配置家庭网络
//		StrObtainStr((u8*)cmd, ssid, '"','"');
//		p = (u8*)strchr((const char*)cmd, ',');
//		StrObtainStr(p, pasword, '"','"');
//		sprintf(at_cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pasword);
//		ESP8266_SendATCmdAck(at_cmd, "OK", 200, 2);//发送配置路由WiFi命令
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
	ESP8266_SendATCmdAck(at_cmd, "OK", 0, 2);//发送配置路由WiFi命令
	
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

/*  AP模式
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
//AP模式
//ssid为空，使用默认SANRISE-XXXX
//password为空，使用默认12345678

void ESP8266_SetAPmode(char *ssid, char *password)
{
#define  BUFCMD_LEN		65
#define  PARA_LEN	20
	extern u8 CPU_GUID[12];
	char bufPara[40] = { 0 };  //保存参数
	char bufCmd[BUFCMD_LEN] = { 0 };  //保存AT指令
	u8 len1,len2;  //长度
	u8 *p;

	
	len1 = strlen(ssid);		//ssid
	len2 = strlen(password);	//密码

	//ESP8266_SendATCmdAck("AT+RESTORE", "OK", 1000,2);			//恢复出厂设置（默认AP模式，IP:192.168.4.1,端口号333）
	
	//os_dly_wait(200);
	if(sysFlag.Bits.WIFI_MODE == 1)
	{
		ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 300, 2);			//=1:sta,=2:ap,=3:ap+sta模式
	}
	else
	{
		ESP8266_SendATCmdAck("AT+CWMODE=3", "OK", 300, 2);			//=1:sta,=2:ap,=3:ap+sta模式
	}
	
	ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//重启模块生效STA配置
	os_dly_wait(200);

	if ((len1 > 0) && (len2 >= 8)) //SSID不为空,密码长度>8
	{
		sprintf(bufCmd, "AT+CWSAP=\"%s\",\"%s\",1,3", ssid,password);	//ssid,密码12345678，通道1，加密方式：WPA2_PSK
	}
	else   //SSID为空，就用Sanrise-xxxx SSID
	{
		//把SSID和MAC加一起，密码默认“12345678”
		p = ESP8266_GetMAC();

		Del_char((char*)p, ':');					//删除字符串中的‘:’
		LowerCharToUpperChar((char*)p, strlen((const char*)p));	//所有小写的字符变成大写

		strcat(bufPara, AP_SSID);
		strcat(bufPara, (const char*)p + 8);				//MAC第5位,MAC第6位
		sprintf(bufCmd, "AT+CWSAP=\"%s\",\"12345678\",1,3", bufPara);
		strcpy((char*)CPU_GUID,bufPara);
	}
	//delay_ms(1000);
	os_dly_wait(100);
	ESP8266_SendATCmdAck(bufCmd, "OK", 400,2);
	//ESP8266_SendATCmdAck("AT+CIPMUX=1", "OK", 300, 2);			//创建多连接
	uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//创建多连接
	delay_ms(300);
	//ESP8266_SendATCmdAck("AT+CIPSERVER=1,8080", "OK", 300, 2);	//打开服务器，设置端口号是8080	
	uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//打开服务器，设置端口号是8080	
	//ChangeMenu(KEY_EXIT);
}



/*  STA Smart C模式
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
//AP模式
void HLK_M30_SetStaSmartCmode() 
{
	HLK_M30_EnterATmode();

	HLK_M30_SendATCmdAck("at+Df=1","ok",200); //恢复默认	
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

//开启模块SmartConfig
void ESP8266_StaSmartConfig(void)
{
		
	ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 200, 2);//STA模式	
	ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//重启模块生效STA配置
	delay_ms(3000);
	ESP8266_SendATCmdAck("AT+CWSTARTSMART", "OK", 200, 2);//开始SmartConfig
	//AT + CWSTOPSMART //停止SmartConfig
}

//模块通过AT指令连接WiFi
void ESP8266_ConnecetWifi(char *ssid, char *password)
{
	char bufCmd[65] = { 0 };  //保存AT指令

	if ((ssid > 0) && (password > 0))
	{
		sprintf(bufCmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
		//ESP8266_SendATCmdAck("AT+CWMODE=1", "OK", 200, 2);//STA模式	
		//uart_sendbuf("AT+CWMODE=1\r\n",0);//STA模式	
		//ESP8266_SendATCmdAck("AT+RST", "OK", 200, 2);//重启模块生效STA配置
		//uart_sendbuf("AT+RST\r\n",0);//重启模块生效STA配置
		delay_ms(3000);
		//ESP8266_SendATCmdAck(bufCmd, "OK", 200, 2);
		uart_sendbuf((u8*)bufCmd,0);
		delay_ms(3000);
		//ESP8266_SendATCmdAck("AT+CIPMUX=1", "OK", 300, 2);			//创建多连接
		uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//创建多连接
		delay_ms(300);
		//ESP8266_SendATCmdAck("AT+CIPSERVER=1,8080", "OK", 300, 2);	//打开服务器，设置端口号是8080	
		uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//打开服务器，设置端口号是8080	
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

//连接WIFI AP
//void HLK_M30_ConnectWifiAP(char *ssid,char *password,u8 sam) 
//{
//#define  BUFCMD_LEN		65
//	char bufCmd[BUFCMD_LEN]={0};  //保存AT指令
//	u8 lenSsid;  //SSID长度
//	u8 lenPassword; //密码长度
//
//	if(HLK_M30_EnterATmode())return; //不能进入AT模式就返回
//
//	lenSsid = strlen(ssid);
//	lenPassword = strlen(password);
//
//	if (lenSsid==0 || lenPassword < 8) //长度不能为0
//	{
//		return;
//	}
//	
//	HLK_M30_SendATCmdAck("at+WM=2","ok",200);
//	HLK_M30_SendATCmdAck("at+WA=0","ok",200);
//
//	//ssid指令
//	strcat(bufCmd,"at+Sssid=");
//	strcat(bufCmd,ssid);
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //设置SSID名称
//
//	//sssidl
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
//	strcat(bufCmd,"at+Sssidl=");
//	strcat(bufCmd,itostr(lenSsid));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SSID长度
//
//	//加密类型
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
//	strcat(bufCmd,"at+Sam=");
//	strcat(bufCmd,itostr(sam));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SAM加密类型
//
//	//密码
//	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
//	strcat(bufCmd,"at+Spw=");
//	strcat(bufCmd,password);
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200); //设置密码
//
//	//密码长度
//	ClrMem((void*)bufCmd,BUFCMD_LEN);
//	strcat(bufCmd,"at+Spwl=");
//	strcat(bufCmd,itostr(lenPassword));
//	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //密码长度
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
//	HLK_M30_SendATCmdAck("at+WC=1","ok",800);//WIFI模块计算PMK时间较长 
//
//	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
//}

void conTowifi(void)
{
	if(HLK_M30_EnterATmode())return; //不能进入AT模式就返回
	
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

//获取本机M30_Buf
u8* HLK_M30_GetIp(void)
{
	u8 *p;
	int star,end,i,j;

	if (HLK_M30_SendATCmdAck("at+CIp=?","CIp",200)==0)
	{
		USART_RX_BUF[USART_RECV_COUNT]='\0';//添加结束符
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

//获取本机模块IP
u8* ESP8266_GetIp(void)
{
//	u8 *p;
//	int star, end, i, j;

	if (ESP8266_SendATCmdAck("AT+CIFSR", "+CIFSR", 200,1) == 0)
	{
		USART_RX_BUF[USART_RECV_COUNT] = '\0';//添加结束符
		StrObtainStr(USART_RX_BUF, M30_Buf,'"','"');
	}
	else
	{
		M30_Buf[0] = '\0';
	}
	return M30_Buf;
}

//获取WIFI名称或连接的AP名称
//u8* HLK_M30_GetWifiName(void)
//{
//	u8 *p;
//
//	p = HLK_M30_GetATCmdAck("at+WA=?",200); //取得WIFI工作模式
//
//	if (*p == '1') //AP模式
//	{
//		p = HLK_M30_GetATCmdAck("at+Assid=?",200);
//	} 
//	else  //其它模式
//	{
//		p = HLK_M30_GetATCmdAck("at+Sssid=?",200);
//	}
//
//	strcpy((char*)&M30_Buf,(char*)p);
//	return M30_Buf;
//}

//获取WIFI名称或连接的AP名称
u8* ESP8266_GetWifiName(void)
{
//	u8 *p;

	if (ESP8266_SendATCmdAck("AT+CWJAP?", "+CWJAP:", 200, 1) == 0) //取得WIFI的相关信息
	{
		StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
	}
	else
	{
		M30_Buf[0] = '\0';
	}
	return M30_Buf;
}

//查询当前WiFi模块的工作模式
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
//获取MAC地址
//返回指向MAC地址数组的指针[] 6个字节
u8* ESP8266_GetMAC(void)
{
//	u8 *p;
//	u8 strMac[4];
//	u8 i,j;
	char wifi_mode;

	wifi_mode = ESP8266_GetWifiMode();

	if (wifi_mode == '1')			//STA模式
	{
		if (ESP8266_SendATCmdAck("AT+CIPSTAMAC?", "OK", 200, 3) == 0)
		{
			ClrMem((void*)M30_Buf, 6);
			StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
		}
	}
	else if ((wifi_mode == '2') || (wifi_mode == '3'))	//wifi模式+STA
	{
		if (ESP8266_SendATCmdAck("AT+CIPAPMAC?", "OK", 200, 3) == 0)
		{
			ClrMem((void*)M30_Buf, 6);
			StrObtainStr(USART_RX_BUF, M30_Buf, '"', '"');
		}
	}
	else 		//其他模式
	{

	}

	//M30_Buf[6]='\0'; //MAC地址只有6位
	return M30_Buf;
}

//IO口初始化
void HLK_M30_IoInit(void)
{

	//GPIO_InitTypeDef  GPIO_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//pin8:上电复位脚;pin11:RST_N;pin12:ES_RST,
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	//GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	//GPIO_SetBits(GPIOA,GPIO_Pin_11|);				 //PB.5 输出高

}

//ESP8266 WiFi模块初始化
void ESP8266_Init(void)
{
	uart_sendbuf("AT+CIPMUX=1\r\n", 0);			//创建多连接
	delay_ms(300);
	uart_sendbuf("AT+CIPSERVER=1,8080\r\n", 0);	//打开服务器，设置端口号是8080	
	ESP8266_SetAPmode("", "");					
	
	//ESP8266_ConnecetWifi("CMCC-FQH","18823831312");
}

//ESP8266发送数据
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

	for (i = 0; i < 5;i++)		//支持5个用户
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
