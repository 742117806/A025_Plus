/******************** (C) COPYRIGHT 2008 VERYTEC ********************
* File Name          : my_type_type.h
* Author             : MCD Application Team
* Version            : V2.0.2
* Date               : 07/11/2008
* Description        : This file contains all the common data types used for the
*                      AT89C51 firmware library.
********************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_TYPE_H_
#define __MY_TYPE_H_

// typedef   bit     BOOL;
 #define 	FALSE   0      
 #define 	TRUE    !FALSE
//typedef enum {FALSE = 0, TRUE = !FALSE}BOOL;

/* Exported functions ------------------------------------------------------- */

#define GetHigByte(X) ((u8)(X>>8)) //ȡ��8λ
#define GetLowByte(X) ((u8)X)	//ȡ��8λ

#define MarkToInt(HBYTE,LBYTE)	(((u16)HBYTE << 8) || LBYTE) //�������ֽںϲ�����
#define ToChar(X)	((u8)X)		//ת���ֽ�
#define ToInt(X)	((u16)X)

//λ����
#define  SETBIT(X) (0x01 << X) //Xλ��1

#define SEND_OS_MSG(X)	(X=TRUE)	//�����ź�
#define CLEAR_OS_MSG(X)	(X=FALSE)	//����ź�

#endif 

/******************* (C) COPYRIGHT 2008 VERYTEC *****************END OF FILE****/
