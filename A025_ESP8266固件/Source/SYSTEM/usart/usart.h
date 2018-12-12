/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM输出操作
@Function List:
@History    : 
<author> <time> <version > <desc>
正点原子
Lennon 2015/8/12 1.0 
***********************************************************/
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "IapProc.h"

#define USART_REC_LEN  			sizeof(UartFrame) //512字节+2字节序号+4字符校验和 = 518字节  //定义最大接收字节数 1K
//#define USART_REC_LEN			1024  //最大不能超过 0X3FFF
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define USART_RECV_COUNT	(USART_RX_STA&0x3FFF)	//当前接收到的长度	
#define USART_RECV_OK		(USART_RX_STA&0x8000)	//接收到一帧数据

#define USART_TYPE_AT	0
#define USART_TYYE_DATA	1
#define USART_TYPE_LED	2

#define SetUartRxType(x)	USART_RX_TYPE=x


typedef struct WiFiRespointstr_
{
	u8 strBuf[16+1];
	u8 cnt;
	u8 rcvOk;
}WiFiRespointstr_t;

extern WiFiRespointstr_t WiFiRespointstr;

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u8  USART_RX_TYPE;               //接收类型
//extern u16 USART_RX_CNT;				//接收的字节数	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound); //串口初始化
void ResetUartRecv(u8 num); //复位串口接收
void uart_sendbuf(u8 *pbuf,u16 len); //如果len = 0,发送字符串,否则发送len个字符

#endif


