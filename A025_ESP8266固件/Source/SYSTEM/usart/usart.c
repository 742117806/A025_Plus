/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 串口处理
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/
#include "sys.h"
#include "usart.h"	  
#include "IapProc.h"
#include "CmdProc.h"
#include "led.h"
#include "ESP8266_07S.h"
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if OS_SUPPORT_RTX
//#include "includes.h"					//ucos 使用	  
#endif

//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20002800)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.  
//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(RAM_APP_ADDR)));//接收缓冲
u8 USART_RX_BUF[USART_REC_LEN];

WiFiRespointstr_t WiFiRespointstr={{0},0};

//接收状态 USART_RX_STA
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;       	//接收状态标记
u8  USART_RX_FRAME_CNT = 0; //收到的帧数
u8  USART_RX_TYPE =	0;  //0:AT指令模式;1:串口升级模式;2:灯控指令模式
u8  buff_link[10] = { 0 };
u8  link_p;
//重置UART接收
//num：接收的帧数
void ResetUartRecv(u8 num)
{
	USART_RX_STA = 0;
	if (num) //num>0
	{
		USART_RX_FRAME_CNT = num-1;
	}
}

//UART接收中断
void USART1_IRQHandler(void)
{
	u8 res;	
	//u8 *p;
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif

#ifdef OS_SUPPORT_RTX  //禁止任务切换
	//tsk_lock();
#endif
//    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//不能用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断
//    {  
//        USART_ReceiveData(USART1);  
//    } 
		
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR; 
//		SEGGER_RTT_printf(0,"%c",res);
		switch(USART_RX_TYPE)
		{
		case USART_TYPE_AT:  //AT指令模式
			WiFiRespointstr.strBuf[WiFiRespointstr.cnt++]=res;
			if(res == '\n')
			{
				WiFiRespointstr.rcvOk=1;
			}
			if(WiFiRespointstr.cnt>16)
			{
				WiFiRespointstr.cnt = 0;
			}
			
			if(USART_RECV_OK==0)//接收未完成
			{
				if(USART_RX_STA&0x4000)//接收到了0x0d
				{
					if (res != 0x0A)  //收到
					{
						USART_RX_STA=0;//接收错误,重新开始
					}
					else 
					{
						if (USART_RX_FRAME_CNT) //没有收到指定的帧数
						{
							USART_RX_FRAME_CNT--;
							USART_RX_STA = USART_RX_STA & 0x3FFF; //清除接收标志，继续接收
							USART_RX_BUF[USART_RX_STA] = '\r';  //添加回车换行符
							USART_RX_BUF[USART_RX_STA+1] = '\n';
							USART_RX_STA +=2;						
						}
						else
						{
							USART_RX_STA |= 0x8000;	//接收完成了 
						}
					}
				}
				else //还没收到0X0D
				{	
					if(res==0x0D)USART_RX_STA|=0x4000;
					else
					{
						USART_RX_BUF[USART_RECV_COUNT]=res;
						USART_RX_STA++;
						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}  
			//USART_RX_BUF[USART_RECV_COUNT] = res;
			//USART_RX_STA++;
			//if (USART_RECV_COUNT > 10)
			//{
			//	USART_RX_STA |= 0x8000;	//接收完成了 
			//}
			break;

		case USART_TYYE_DATA:  //升级数据模式
			if(USART_RX_STA< (USART_REC_LEN-1))
			{
				USART_RX_BUF[USART_RX_STA++]=res;
			}
			else
			{
				USART_RX_BUF[USART_RX_STA]=res;
				USART_RX_STA |=0x8000;  //接收完成了 
			}
			break;

		case USART_TYPE_LED: //灯控指令模式
			
			WiFiRespointstr.strBuf[WiFiRespointstr.cnt++]=res;
			if(res == '\n')
			{
				WiFiRespointstr.rcvOk=1;
			}
			if(WiFiRespointstr.cnt>16)
			{
				WiFiRespointstr.cnt = 0;
			}

			if (USART_RX_STA < (USART_REC_LEN - 1))
			{
				if (res == FRAME_START)
				{
					USART_RX_STA = 0; //重新接收
				}
				USART_RX_BUF[USART_RX_STA++] = res;

				if (res == FRAME_END) //收到结束
				{
					USART_RX_STA |= 0x8000;  //接收完成了 

				}
			}
			else
			{
				USART_RX_STA = 0;
			}
			if (link_p < (10 - 1))
			{
				if (res == '+')
				{
					link_p = 0;
				}
				buff_link[link_p++] = res;
				if (link_p>7)
				{
					if (memcmp(buff_link, "+IPD,", 5) == 0)
					{
						current_linkID = buff_link[5] - '0';
					}
					//else if (memcmp(buff_link, "+CWJAP:\"", 8) == 0)
					//{
					//	wifiConnetFlag = 1;
					//}
					//else if (strstr(USART_RX_BUF, "No AP") != NULL)
					//{
					//	wifiConnetFlag = 0;
					//}
					link_p = 0;
				}

			}
			
			break;
		}			 									     
	}
	//else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//接收到一帧数据  
	//{  
	//	USART1->SR;//先读SR  
	//	USART1->DR;//再读DR 
	//	//printf("%s", USART_RX_BUF);
	//	if ((memcmp(USART_RX_BUF, "0,CONNECT",9) == 0)||
	//		(memcmp(USART_RX_BUF, "0,CLOSED", 8) == 0))
	//	{
	//		//printf("123456");
	//		USART_RX_STA = 0; //重新接收
	//		LED_WIFI ^= 1;
	//	}
	//}


#ifdef OS_SUPPORT_RTX  //禁止任务切换
//	tsk_unlock();
#endif

#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	
void uart_init(u32 bound){
    //GPIO端口设置
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
#if EN_USART1_RX		  //如果使能了接收

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	/* 使能空闲中断*/
	//USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}

//串口发送数据
//如果len = 0，发送字符串,否则发送len个字符
void uart_sendbuf(u8 *buf,u16 len)
{
	if (len) //发送指定数量的字符
	{
		while(len--)
		{
			while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			USART1->DR = *buf++;   
		}
	}
	else
	{
		while(*buf)  //发送字符串
		{
			while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			USART1->DR = *buf++;   
		}
	}
}


 
 

