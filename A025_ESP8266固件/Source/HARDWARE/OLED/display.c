/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 显示屏显示 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "display.h"
#include "oled.h"
#include "delay.h"

extern u8 OLED_GRAM[OLED_SEG][OLED_PAGE];

/*****************************************************************************
*@brief :初始化显示信息
*@参娄  :x 列 y行 speed 速度 
****************************************************************************/
#define LCDNum   16
void LCDMoveDisplay(unsigned char NUM,char const *s1,char const *s2)                     
{
	unsigned char i = 0,j = 0;
	unsigned char Buf1[LCDNum]={0};
	unsigned char Buf2[LCDNum]={0};

	while(*s1)
	{
		for(i = 0;i< LCDNum;i++)
		{
			Buf1[i] = *(s1+i);
			Buf2[i] = *(s2+i);
		}
		Buf1[LCDNum-1]='\0';
		Buf2[LCDNum-1]='\0';
	 	OLED_ShowString(0,1,Buf1);
		OLED_ShowString(0,16,Buf2);
		OLED_Refresh_Gram();
		delay_ms(80);
		if(++j == NUM){
			delay_ms(1200);
			delay_ms(1200);
		}
		s1++;
		s2++;
	}
}

/***********************************************************************************
  *@brief:初始化显示信息
	*@返回:
	*********************************************************************************/
void initMessage(void)
{	
	LCDMoveDisplay(LCDNum-2,
				"                 SANRISE             ",  
				"              Aquarium Light         "
                );
}


/*-----------------------------
在屏幕中间绘制一副128*32图片 
参数:图片数据地址
------------------------------*/
void OLED_MoveImg128X32(const u8 *pic)
{
	u8 cX;
	u8 i;
	u8 yt; //

	OLED_Clear_Ram();
	yt = (OLED_PAGE-4)/2; //计算出中间位置偏移

	for (i=1;i<128;i++)  //逐渐显示
	{
		for (cX=0;cX<i;cX++)
		{
			OLED_GRAM[cX][3+yt] = *(pic + cX+0);
			OLED_GRAM[cX][2+yt] = *(pic + cX+128);
			OLED_GRAM[cX][1+yt] = *(pic + cX+256);
			OLED_GRAM[cX][0+yt] = *(pic + cX+384);
		}
		OLED_Refresh_Gram();
		delay_ms(10);
	}
	delay_ms(2500); //暂停

	//擦除
	for (i=127;i>0;i--)
	{
		for (cX=127;cX>i;cX--)
		{
			OLED_GRAM[cX][3+yt] = 0x00;
			OLED_GRAM[cX][2+yt] = 0x00;
			OLED_GRAM[cX][1+yt] = 0x00;
			OLED_GRAM[cX][0+yt] = 0x00;
		}
		OLED_Refresh_Gram();
		delay_ms(10);
	}
}


/*-----------------------------
显示图片
参数:图片数据地址
------------------------------*/
void ShowImg128X32(const u8 *pic)
{
	u8 cX;
	u8 i;
	u8 yt; //

	OLED_Clear_Ram();
	yt = (OLED_PAGE-4)/2; //计算出中间位置偏移

	for (i=1;i<128;i++)  //逐渐显示
	{
		for (cX=0;cX<i;cX++)
		{
			OLED_GRAM[cX][3+yt] = *(pic + cX+0);
			OLED_GRAM[cX][2+yt] = *(pic + cX+128);
			OLED_GRAM[cX][1+yt] = *(pic + cX+256);
			OLED_GRAM[cX][0+yt] = *(pic + cX+384);
		}
		OLED_Refresh_Gram();
		delay_ms(10);
	}
}

/*-----------------------------
擦除图片
参数:无
------------------------------*/
void ErasureImg128X32(void)
{
	u8 cX;
	u8 i;
	u8 yt; //
	//擦除
	for (i=127;i>0;i--)
	{
		for (cX=127;cX>i;cX--)
		{
			OLED_GRAM[cX][3+yt] = 0x00;
			OLED_GRAM[cX][2+yt] = 0x00;
			OLED_GRAM[cX][1+yt] = 0x00;
			OLED_GRAM[cX][0+yt] = 0x00;
		}
		OLED_Refresh_Gram();
		delay_ms(10);
	}
}
/*---------end of file---------*/
