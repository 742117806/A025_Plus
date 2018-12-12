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

#define GetHigByte(X) ((u8)(X>>8)) //取高8位
#define GetLowByte(X) ((u8)X)	//取低8位

#define MarkToInt(HBYTE,LBYTE)	(((u16)HBYTE << 8) || LBYTE) //将两个字节合并成字
#define ToChar(X)	((u8)X)		//转到字节
#define ToInt(X)	((u16)X)

//位操作
#define  SETBIT(X) (0x01 << X) //X位置1

#define SEND_OS_MSG(X)	(X=TRUE)	//发送信号
#define CLEAR_OS_MSG(X)	(X=FALSE)	//清除信号

#endif 

/******************* (C) COPYRIGHT 2008 VERYTEC *****************END OF FILE****/
