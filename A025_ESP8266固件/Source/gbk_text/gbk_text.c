/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: menu_cn.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-10-24
@Description: 汉字显示
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

u32 FONTINFOADDR = W25QXX_FONT_ADDR;//默认是6M的地址

extern u8 OLED_GRAM[OLED_SEG][OLED_PAGE];	
					            
//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 size*2 bytes大小	 
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	qh=*code;
	ql=*(++code);

	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<(size*2);i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*(size*2);//得到字库中的字节偏移量  		  

	SPI_Flash_Read(mat,FONTINFOADDR+foffset,32);
}  
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	   
void Show_Font(u8 x,u8 y,u8 *font,u8 mode)
{
	u8 dzk[32];
	Get_HzMat(font,(u8*)&dzk,16);//得到相应大小的点阵数据
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
		if(byteH < 0x80) //是ASCII字符
		{
			OLED_ShowChar(pX,pY*16,*str,16,1);	 
			pX += 8;
			str++;			
		}
		else  //中文字符
		{
			Show_Font(pX,pY*2,str,1);
			str+=2;
			pX += 16;
		}
	}  
}	

/*-----------------------------
绘制一副16*16图片
参数:显示的x,y和图片数据地址
X=0~3;
Y=0~127;
取模方向: 
1357..
2468..
------------------------------*/
void OLED_DrawFont16X16(u8 Start_X,u8 Start_Y,const u8 *pic)
{
	u8 i;
	u8 x,y;
	y = 3-Start_Y;
	for (i=0;i<16;i++)  //第一行
	{
		x = (Start_X+i)%128;
		OLED_GRAM[x][y] = *pic++;
		OLED_GRAM[x][y-1] = *pic++;
	}
}









		  






