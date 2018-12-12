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

#include "HLK_M30.h"
//#include "include.h"
#include <string.h>
#include <ctype.h>
#include <MATH.H>
#include "my_string.h"
#include "usart.h"
#include "delay.h"
#include "rtx_os.h"

#define Delay_AT_10ms(X)	OS_delay_10ms(TMR_OTHER,X)

u8 M30_Buf[26];


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
	//USART_RX_STA=0; //接收清0
	
	if(ack&&cntTimes)		//需要等待应答
	{
		while(--cntTimes)	//等待倒计时
		{
			Delay_AT_10ms(1);
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
void HLK_M30_Reset()
{
	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
}

//复位--硬件方式
void HLK_M30_HWReset()
{
	M30_reset = 0;
	//delay_ms(500);
	Delay_AT_10ms(50);
	M30_reset = 1;
	HLK_M30_PowerUp();
}

//上电时序
void HLK_M30_PowerUp(void)
{
	M30_reset = 0;
	M30_RX = 0;
//	delay_ms(200);
	Delay_AT_10ms(20);
	M30_reset = 1;
//	delay_ms(60);
	Delay_AT_10ms(60);
	M30_RX = 1;
}

//恢复默认-硬件方式
void HLK_M30_HWLoadDefault()
{
	u8 i;
	M30_Exit_Default = 0;
	i = 10;   //6秒以上复位
	while(i--)
	{
		//delay_ms(1000);
		Delay_AT_10ms(100);
	}
	M30_Exit_Default = 1;

}

//恢复默认
void HLK_M30_LoadDefault()
{
	/*
	u8 i;
	M30_Exit_Default = 0;
	i = 10;   //6秒以上复位
	while(i--)
	{
		delay_ms(1000);
	}
	M30_Exit_Default = 1;
	*/
	HLK_M30_EnterATmode();
	HLK_M30_SendATCmdAck("at+Df=1","ok",200); //恢复默认
	HLK_M30_SendATCmdAck("at+Rb=1","ok",200); //重启

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
void HLK_M30_SetAPmode(char *ssid,char *password) 
{
#define  BUFCMD_LEN		65
#define  PARA_LEN	20
	char bufPara[40]={0};  //保存参数
	char bufCmd[BUFCMD_LEN]={0};  //保存AT指令
	u8 len;  //长度
	u8 *p;

	if(HLK_M30_EnterATmode())return; //不能进入AT模式就返回

//	HLK_M30_SendATCmdAck("at+Df=1","ok",200); //恢复默认

	len = strlen(ssid);
	if (len) //SSID不为空,就用ssid
	{
		strcpy(bufPara,ssid);
	}
	else   //SSID为空，就用Sanrise-xxxx SSID
	{
		//把SSID和MAC加一起
		p = HLK_M30_GetMAC();
		
		strcat(bufPara,AP_SSID);	
		strcat(bufPara,strhex(*(p+4))); //MAC第5位
		strcat(bufPara,strhex(*(p+5))); //MAC第6位
	}

	//ssid指令
	strcat(bufCmd,"at+Assid=");
	strcat(bufCmd,bufPara);
	
	HLK_M30_SendATCmdAck("at+WA=1","ok",200);
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //设置WIFI名称
/****************************
	//HLK_M30_SendATCmdAck("at+Assid=Sinrase-XXXX","ok",200);
	//HLK_M30_SendATCmdAck("at+Assidl=12","ok",200);
****************************/

	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
	len = strlen(bufPara);
	strcat(bufCmd,"at+Assidl=");
	strcat(bufCmd,itostr(len));
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SSID长度

	HLK_M30_SendATCmdAck("at+Aam=9","ok",200);  //9:WPA/WPA2_AES加密

	//密码
	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
	ClrMem((void*)bufPara,PARA_LEN);
	strcat(bufCmd,"at+Apw=");
	len = strlen(password);
	if (len>=8)strcat(bufPara,password);  //指定的密码，密码长度必须8位以上
	else strcat(bufPara,AP_PASSWORD);  //默认的密码
	
	strcat(bufCmd,bufPara);
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200); //设置密码

	ClrMem((void*)bufCmd,BUFCMD_LEN);
	len = strlen(bufPara);
	strcat(bufCmd,"at+Apwl=");
	strcat(bufCmd,itostr(len));
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //密码长度

/**********************
	HLK_M30_SendATCmdAck("at+Apw=12345678","ok",200);  //密码
	HLK_M30_SendATCmdAck("at+Apwl=8","ok",200);  //密码长度
**********************/

	HLK_M30_SendATCmdAck("at+dhcp=0","ok",200);
	HLK_M30_SendATCmdAck("at+Aip=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+mask=255,255,255,0","ok",200);
	HLK_M30_SendATCmdAck("at+gw=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+dns=192,168,0,99","ok",200);
	HLK_M30_SendATCmdAck("at+UType=1","ok",200);
	//HLK_M30_SendATCmdAck("at+UIp=192.168.0.1","ok",200);
	HLK_M30_SendATCmdAck("at+UIp=0","ok",200);
	HLK_M30_SendATCmdAck("at+URPort=0","ok",200);
	HLK_M30_SendATCmdAck("at+ULPort=8080","ok",200);

	HLK_M30_SendATCmdAck("at+Ub=115200","ok",200);	
	HLK_M30_SendATCmdAck("at+Ud=8","ok",200);
	HLK_M30_SendATCmdAck("at+Up=0","ok",200);
	HLK_M30_SendATCmdAck("at+Us=1","ok",200);
	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
	
//	OLED_ShowString(0,0,"Set ap mode");
//	OLED_Refresh_Gram();
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
void HLK_M30_ConnectWifiAP(char *ssid,char *password,u8 sam) 
{
#define  BUFCMD_LEN		65
	char bufCmd[BUFCMD_LEN]={0};  //保存AT指令
	u8 lenSsid;  //SSID长度
	u8 lenPassword; //密码长度

	if(HLK_M30_EnterATmode())return; //不能进入AT模式就返回

	lenSsid = strlen(ssid);
	lenPassword = strlen(password);

	if (lenSsid==0 || lenPassword < 8) //长度不能为0
	{
		return;
	}
	
	HLK_M30_SendATCmdAck("at+WM=2","ok",200);
	HLK_M30_SendATCmdAck("at+WA=0","ok",200);

	//ssid指令
	strcat(bufCmd,"at+Sssid=");
	strcat(bufCmd,ssid);
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //设置SSID名称

	//sssidl
	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
	strcat(bufCmd,"at+Sssidl=");
	strcat(bufCmd,itostr(lenSsid));
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SSID长度

	//加密类型
	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
	strcat(bufCmd,"at+Sam=");
	strcat(bufCmd,itostr(sam));
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //SAM加密类型

	//密码
	ClrMem((void*)bufCmd,BUFCMD_LEN);  //清除内存
	strcat(bufCmd,"at+Spw=");
	strcat(bufCmd,password);
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200); //设置密码

	//密码长度
	ClrMem((void*)bufCmd,BUFCMD_LEN);
	strcat(bufCmd,"at+Spwl=");
	strcat(bufCmd,itostr(lenPassword));
	HLK_M30_SendATCmdAck((u8*)bufCmd,"ok",200);  //密码长度

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
	HLK_M30_SendATCmdAck("at+WC=1","ok",800);//WIFI模块计算PMK时间较长 

	HLK_M30_SendATCmdAck("at+Rb=1","ok",200);
}

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

//获取WIFI名称或连接的AP名称
u8* HLK_M30_GetWifiName(void)
{
	u8 *p;

	p = HLK_M30_GetATCmdAck("at+WA=?",200); //取得WIFI工作模式

	if (*p == '1') //AP模式
	{
		p = HLK_M30_GetATCmdAck("at+Assid=?",200);
	} 
	else  //其它模式
	{
		p = HLK_M30_GetATCmdAck("at+Sssid=?",200);
	}

	strcpy((char*)&M30_Buf,(char*)p);
	return M30_Buf;
}

//获取MAC地址
//返回指向MAC地址数组的指针[] 6个字节
u8* HLK_M30_GetMAC(void)
{
	u8 *p;
	u8 strMac[4];
	u8 i,j;

	p = HLK_M30_GetATCmdAck("at+mac=?",200);

	if (p != USART_RX_BUF)
	{
		ClrMem((void*)M30_Buf,6);
		for(i=0;i<6;i++)
		{
			ClrMem((void*)strMac,4);
			for(j=0;j<4;j++)
			{
				if (*p == ',' || *p == '\0') //'号或是结束
				{
					p++;
					M30_Buf[i] = strdec((char*)strMac);
					break;
				}
				else
				{
					strMac[j] = *p++;
				}
			}
		}
	}
	M30_Buf[6]='\0'; //MAC地址只有6位

	return M30_Buf;
}

//IO口初始化
void HLK_M30_IoInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;	//pin8:上电复位脚;pin11:RST_N;pin12:ES_RST,
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12);				 //PB.5 输出高

/*
	//USART1_TX   PA.9
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_9);	
	*/
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
