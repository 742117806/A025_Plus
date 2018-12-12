/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ��ʾ����ʾ 
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
*@brief :��ʼ����ʾ��Ϣ
*@��¦  :x �� y�� speed �ٶ� 
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
  *@brief:��ʼ����ʾ��Ϣ
	*@����:
	*********************************************************************************/
void initMessage(void)
{	
	LCDMoveDisplay(LCDNum-2,
				"                 SANRISE             ",  
				"              Aquarium Light         "
                );
}


/*-----------------------------
����Ļ�м����һ��128*32ͼƬ 
����:ͼƬ���ݵ�ַ
------------------------------*/
void OLED_MoveImg128X32(const u8 *pic)
{
	u8 cX;
	u8 i;
	u8 yt; //

	OLED_Clear_Ram();
	yt = (OLED_PAGE-4)/2; //������м�λ��ƫ��

	for (i=1;i<128;i++)  //����ʾ
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
	delay_ms(2500); //��ͣ

	//����
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
��ʾͼƬ
����:ͼƬ���ݵ�ַ
------------------------------*/
void ShowImg128X32(const u8 *pic)
{
	u8 cX;
	u8 i;
	u8 yt; //

	OLED_Clear_Ram();
	yt = (OLED_PAGE-4)/2; //������м�λ��ƫ��

	for (i=1;i<128;i++)  //����ʾ
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
����ͼƬ
����:��
------------------------------*/
void ErasureImg128X32(void)
{
	u8 cX;
	u8 i;
	u8 yt; //
	//����
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
