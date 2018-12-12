/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: OLED显示屏驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE			0
#define OLED_PANEL_TYPE		0	//1:128x64  0:128x32
#define OLED_SPI_TYPE		1	//1:硬件SPI，0：IO口模拟SPI 

#define	OLED_SEG	128  //列
#define OLED_MAX_X	127
#if OLED_PANEL_TYPE
#define OLED_PAGE	8  //页
#define OLED_MAX_Y	63
#else
#define OLED_PAGE	4  //页
#define OLED_MAX_Y	31
#endif

//-----------------OLED端口定义----------------  					   
#if OLED_MODE
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_2)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_8)  //SPI:RST
#define OLED_RST_Set() GPIO_SetBits(GPIOA,GPIO_Pin_8)

#define OLED_RS_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_4)  //SPI:DC
#define OLED_RS_Set() GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define OLED_WR_Clr() GPIO_ResetBits(GPIOG,GPIO_Pin_14)
#define OLED_WR_Set() GPIO_SetBits(GPIOG,GPIO_Pin_14)

#define OLED_RD_Clr() GPIO_ResetBits(GPIOG,GPIO_Pin_13)
#define OLED_RD_Set() GPIO_SetBits(GPIOG,GPIO_Pin_13)
//PC0~7,作为数据线
#define DATAOUT(x) GPIO_Write(GPIOC,x);//输出  
//使用4线串行接口时使用 
#else
#if OLED_SPI_TYPE
/*------------
PA2  -- CS 
PA3  -- RST
PA4  -- D/C  RS
PA5  -- SCLK
PA6  -- 空
PA7  -- SDIN
--------------*/


/*
#define OLED_RST_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_3)  //SPI:RST
#define OLED_RST_Set() GPIO_SetBits(GPIOA,GPIO_Pin_3)

#define OLED_RS_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_4)  //SPI:DC
#define OLED_RS_Set() GPIO_SetBits(GPIOA,GPIO_Pin_4)


#define OLED_CS_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define OLED_CS_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_2)
*/
#define OLED_CS		PAout(2)
#define OLED_RST	PAout(3)
#define OLED_RS		PAout(4)

#define OLED_RST_Clr()	OLED_RST=0  //SPI:RST
#define OLED_RST_Set()  OLED_RST=1

#define OLED_RS_Clr()  OLED_RS=0  //SPI:DC
#define OLED_RS_Set()  OLED_RS=1

#define OLED_CS_Clr()  OLED_CS=0
#define OLED_CS_Set()  OLED_CS=1

#else

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5) //SPI:CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7) //SPI:DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)
#endif
#endif

//字体大小
#define FONT16	16
#define FONT12	12
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_DrawImg16X16(u8 Start_X, u8 Start_Y, const u8 *pic);
void OLED_Clear(void);
void OLED_Clear_Ram(void); //清除显存
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);
void OLED_ShowStringA(u8 x,u8 y,const u8 *p,u8 mode);

//===============
void OLED_DrawImg128X16(const u8 *pic);
void OLED_FLIP(u8 x,u8 y,u8 len);
void OLED_DrawImg128X32(const u8 *pic);

#endif  
	 

/*---------end of file---------*/

