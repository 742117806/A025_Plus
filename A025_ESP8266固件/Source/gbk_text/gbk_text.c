/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: menu_cn.h
@Author  : ������
@Version : 1.0
@Date    : 2015-10-24
@Description: ������ʾ
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "sys.h" 
#include "flash.h"
#include "oled.h"
#include "gbk_text.h"	
#include "IapProc.h"
#include "system.h"

u32 FONTINFOADDR = W25QXX_FONT_ADDR;//Ĭ����6M�ĵ�ַ

extern u8 OLED_GRAM[OLED_SEG][OLED_PAGE];	
					            
//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,GBK��
//mat  ���ݴ�ŵ�ַ size*2 bytes��С	 
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	qh=*code;
	ql=*(++code);

	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{   		    
	    for(i=0;i<(size*2);i++)*mat++=0x00;//�������
	    return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*(size*2);//�õ��ֿ��е��ֽ�ƫ����  		  

	SPI_Flash_Read(mat,FONTINFOADDR+foffset,32);
}  
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	   
void Show_Font(u8 x,u8 y,u8 *font,u8 mode)
{
	u8 dzk[32];
	Get_HzMat(font,(u8*)&dzk,16);//�õ���Ӧ��С�ĵ�������
	OLED_DrawFont16X16(x,y,dzk);

}

//x=0~127-
void OLED_ShowStringCn(u8 x,u8 y,u8 *str)
{
	u8 byteH;
	u8 pX,pY;

	pX = x % 127;
	pY = y % 2;
	
	while(*str !='\0')
	{       
		byteH = *str;
		if(byteH < 0x80) //��ASCII�ַ�
		{
			OLED_ShowChar(pX,pY*16,*str,16,1);	 
			pX += 8;
			str++;			
		}
		else  //�����ַ�
		{
			Show_Font(pX,pY*2,str,1);
			str+=2;
			pX += 16;
		}
	}  
}	

/*-----------------------------
����һ��16*16ͼƬ
����:��ʾ��x,y��ͼƬ���ݵ�ַ
X=0~3;
Y=0~127;
ȡģ����: 
1357..
2468..
------------------------------*/
void OLED_DrawFont16X16(u8 Start_X,u8 Start_Y,const u8 *pic)
{
	u8 i;
	u8 x,y;
	y = 3-Start_Y;
	for (i=0;i<16;i++)  //��һ��
	{
		x = (Start_X+i)%128;
		OLED_GRAM[x][y] = *pic++;
		OLED_GRAM[x][y-1] = *pic++;
	}
}









		  






