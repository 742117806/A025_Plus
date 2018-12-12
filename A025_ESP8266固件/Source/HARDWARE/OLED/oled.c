/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: OLED显示屏驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

#include "stm32f10x_spi.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
u8 OLED_GRAM[OLED_SEG][OLED_PAGE];	 

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<OLED_PAGE;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<OLED_SEG;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();		   
	OLED_CS_Clr();
	OLED_WR_Clr();	 
	OLED_WR_Set();
	OLED_CS_Set();	  
	OLED_RS_Set();	 
} 	    	    
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
//	u8 i;
	u16 retry=0;			

	if(cmd) OLED_RS_Set();
	else OLED_RS_Clr();		  
	
	OLED_CS_Clr();
	SPI_I2S_SendData(SPI1,dat); //SPI 发送数据	 	

	//BUG: SPI_I2S_GetFlagStatus这里执行有问题。SPI在低速率传输时，数据未发送完成，就已经执行了CS置位，导致OLED显示异常
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>1)break;
	}
	OLED_CS_Set();
 // OLED_RS_Set();   	  	
} 
#endif
	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	OLED_Clear_Ram();
 	OLED_Refresh_Gram();//更新显示
}

void OLED_Clear_Ram(void)
{
	u8 i,n;  
	for(i=0;i<OLED_PAGE;i++)for(n=0;n<OLED_SEG;n++)OLED_GRAM[n][i]=0x00; 
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>OLED_MAX_X||y>OLED_MAX_Y)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==FONT12)temp=oled_asc2_1206[chr][t];  //调用1206字体
		else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#if OLED_PANEL_TYPE
#define MAX_CHAR_POSY 58 
#else
#define MAX_CHAR_POSY 26 
#endif
         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear_Ram();} //OLED_Clear
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	  

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
//mode: 0：反显，1:正常
void OLED_ShowStringA(u8 x,u8 y,const u8 *p,u8 mode)
{
#define MAX_CHAR_POSX 122
#if OLED_PANEL_TYPE
#define MAX_CHAR_POSY 58 
#else
#define MAX_CHAR_POSY 26 
#endif
         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear_Ram();} //OLED_Clear
        OLED_ShowChar(x,y,*p,16,mode);	 
        x+=8;
        p++;
    }  
}
//初始化SSD1306					    
void OLED_Init(void)
{ 	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC,D,G端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );	//SPI1时钟使能 	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;				 //RST  RS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //上拉输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);						 //输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;				 //RST  RS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //PA5/6/7复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);		 //输出高

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	SPI_SSOutputCmd(SPI1,ENABLE); //使能SPI NSS管脚,
//SPI_I2S_ITConfig(SPI1,DISABLE);
  							  
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示

	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率

	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X1F,OLED_CMD); //默认0X3F(1/64) 

	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭

	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;

	OLED_WR_Byte(0xC8,OLED_CMD); //*COM输出方向 C8

	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x02,OLED_CMD); //[5:4]配置

	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0x8F,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)

	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xF1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;

	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x40,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();
}  

//=======================================
/*-----------------------------
绘制一副16*16图片
参数:显示的x,y和图片数据地址
------------------------------*/
void OLED_DrawImg16X16(u8 Start_X,u8 Start_Y,const u8 *pic)
{
	u8 cX,cY;
	cY = 7-Start_Y;
	for (cX=0;cX<16;cX++)  //第一行
	{
		OLED_GRAM[(Start_X+cX)%128][cY] = *pic++;
	}

	cY -= 1;
	for (cX=0;cX<16;cX++) //第二行
	{
		OLED_GRAM[(Start_X+cX)%128][cY] = *pic++;
	}
}

/*-----------------------------
绘制一副24*24图片
参数:显示的x,y和图片数据地址
------------------------------*/
void OLED_DrawImg24X24(u8 Start_X,u8 Start_Y,const u8 *pic)
{
	u8 cX;
	//if(Start_X > 103)Start_X=0; // 128-24 = 104
	Start_Y = 7-Start_Y;
	if (Start_Y>5)Start_Y = 7;

	for (cX=0;cX<24;cX++)
	{
		OLED_GRAM[(Start_X+cX)%128][Start_Y] = *pic++;
	}

	for (cX=0;cX<24;cX++)
	{
		OLED_GRAM[(Start_X+cX)%128][Start_Y-1] = *pic++;
	}

	for (cX=0;cX<24;cX++)
	{
		OLED_GRAM[(Start_X+cX)%128][Start_Y-2] = *pic++;
	}

}

/*-----------------------------
绘制一副128*64图片 
参数:图片数据地址
------------------------------*/
void OLED_DrawImg128X32(const u8 *pic)
{
	u8 cX,Start_Y;

	Start_Y = 4;
	while(Start_Y)
	{
		for (cX=0;cX<128;cX++)
		{
			OLED_GRAM[cX][Start_Y-1] = *pic++;
		}
		Start_Y--;
	}
}

/*-----------------------------
绘制一副128*16图片 
参数:图片数据地址
------------------------------*/
void OLED_DrawImg128X16(const u8 *pic)
{
	u8 cX,Start_Y;

	Start_Y = 2;
	while(Start_Y)
	{
		for (cX=0;cX<128;cX++)
		{
			OLED_GRAM[cX][Start_Y-1] = *pic++;
		}
		Start_Y--;
	}
}

//反白指定区域
//x=0~127
//y=0~1
void OLED_FLIP(u8 x,u8 y,u8 len)
{
	u8 i;
	if (len >= OLED_SEG)len = OLED_SEG-1;
	y = (OLED_PAGE-1)-y*2; //一次显示2行

	for(i=0;i<len;i++)
	{
		OLED_GRAM[x+i][y] = ~OLED_GRAM[x+i][y];
		OLED_GRAM[x+i][y-1] = ~OLED_GRAM[x+i][y-1];
	}
}

/*---------end of file---------*/
