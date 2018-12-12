/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: PWM�������
@Function List:
@History    : 
<author> <time> <version > <desc>
����ԭ��
Lennon 2015/8/12 1.0 
***********************************************************/
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "IapProc.h"

#define USART_REC_LEN  			sizeof(UartFrame) //512�ֽ�+2�ֽ����+4�ַ�У��� = 518�ֽ�  //�����������ֽ��� 1K
//#define USART_REC_LEN			1024  //����ܳ��� 0X3FFF
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define USART_RECV_COUNT	(USART_RX_STA&0x3FFF)	//��ǰ���յ��ĳ���	
#define USART_RECV_OK		(USART_RX_STA&0x8000)	//���յ�һ֡����

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

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern u8  USART_RX_TYPE;               //��������
//extern u16 USART_RX_CNT;				//���յ��ֽ���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound); //���ڳ�ʼ��
void ResetUartRecv(u8 num); //��λ���ڽ���
void uart_sendbuf(u8 *pbuf,u16 len); //���len = 0,�����ַ���,������len���ַ�

#endif


