/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ���ڴ���
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
//���ʹ��ucos,����������ͷ�ļ�����.
#if OS_SUPPORT_RTX
//#include "includes.h"					//ucos ʹ��	  
#endif

//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20002800)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.  
//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(RAM_APP_ADDR)));//���ջ���
u8 USART_RX_BUF[USART_REC_LEN];

WiFiRespointstr_t WiFiRespointstr={{0},0};

//����״̬ USART_RX_STA
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;       	//����״̬���
u8  USART_RX_FRAME_CNT = 0; //�յ���֡��
u8  USART_RX_TYPE =	0;  //0:ATָ��ģʽ;1:��������ģʽ;2:�ƿ�ָ��ģʽ
u8  buff_link[10] = { 0 };
u8  link_p;
//����UART����
//num�����յ�֡��
void ResetUartRecv(u8 num)
{
	USART_RX_STA = 0;
	if (num) //num>0
	{
		USART_RX_FRAME_CNT = num-1;
	}
}

//UART�����ж�
void USART1_IRQHandler(void)
{
	u8 res;	
	//u8 *p;
#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntEnter();    
#endif

#ifdef OS_SUPPORT_RTX  //��ֹ�����л�
	//tsk_lock();
#endif
//    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//������if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)���ж�
//    {  
//        USART_ReceiveData(USART1);  
//    } 
		
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR; 
//		SEGGER_RTT_printf(0,"%c",res);
		switch(USART_RX_TYPE)
		{
		case USART_TYPE_AT:  //ATָ��ģʽ
			WiFiRespointstr.strBuf[WiFiRespointstr.cnt++]=res;
			if(res == '\n')
			{
				WiFiRespointstr.rcvOk=1;
			}
			if(WiFiRespointstr.cnt>16)
			{
				WiFiRespointstr.cnt = 0;
			}
			
			if(USART_RECV_OK==0)//����δ���
			{
				if(USART_RX_STA&0x4000)//���յ���0x0d
				{
					if (res != 0x0A)  //�յ�
					{
						USART_RX_STA=0;//���մ���,���¿�ʼ
					}
					else 
					{
						if (USART_RX_FRAME_CNT) //û���յ�ָ����֡��
						{
							USART_RX_FRAME_CNT--;
							USART_RX_STA = USART_RX_STA & 0x3FFF; //������ձ�־����������
							USART_RX_BUF[USART_RX_STA] = '\r';  //��ӻس����з�
							USART_RX_BUF[USART_RX_STA+1] = '\n';
							USART_RX_STA +=2;						
						}
						else
						{
							USART_RX_STA |= 0x8000;	//��������� 
						}
					}
				}
				else //��û�յ�0X0D
				{	
					if(res==0x0D)USART_RX_STA|=0x4000;
					else
					{
						USART_RX_BUF[USART_RECV_COUNT]=res;
						USART_RX_STA++;
						if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}  
			//USART_RX_BUF[USART_RECV_COUNT] = res;
			//USART_RX_STA++;
			//if (USART_RECV_COUNT > 10)
			//{
			//	USART_RX_STA |= 0x8000;	//��������� 
			//}
			break;

		case USART_TYYE_DATA:  //��������ģʽ
			if(USART_RX_STA< (USART_REC_LEN-1))
			{
				USART_RX_BUF[USART_RX_STA++]=res;
			}
			else
			{
				USART_RX_BUF[USART_RX_STA]=res;
				USART_RX_STA |=0x8000;  //��������� 
			}
			break;

		case USART_TYPE_LED: //�ƿ�ָ��ģʽ
			
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
					USART_RX_STA = 0; //���½���
				}
				USART_RX_BUF[USART_RX_STA++] = res;

				if (res == FRAME_END) //�յ�����
				{
					USART_RX_STA |= 0x8000;  //��������� 

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
	//else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//���յ�һ֡����  
	//{  
	//	USART1->SR;//�ȶ�SR  
	//	USART1->DR;//�ٶ�DR 
	//	//printf("%s", USART_RX_BUF);
	//	if ((memcmp(USART_RX_BUF, "0,CONNECT",9) == 0)||
	//		(memcmp(USART_RX_BUF, "0,CLOSED", 8) == 0))
	//	{
	//		//printf("123456");
	//		USART_RX_STA = 0; //���½���
	//		LED_WIFI ^= 1;
	//	}
	//}


#ifdef OS_SUPPORT_RTX  //��ֹ�����л�
//	tsk_unlock();
#endif

#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	
void uart_init(u32 bound){
    //GPIO�˿�����
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
#if EN_USART1_RX		  //���ʹ���˽���

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	/* ʹ�ܿ����ж�*/
	//USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#endif
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

//���ڷ�������
//���len = 0�������ַ���,������len���ַ�
void uart_sendbuf(u8 *buf,u16 len)
{
	if (len) //����ָ���������ַ�
	{
		while(len--)
		{
			while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
			USART1->DR = *buf++;   
		}
	}
	else
	{
		while(*buf)  //�����ַ���
		{
			while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
			USART1->DR = *buf++;   
		}
	}
}


 
 

