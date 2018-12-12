/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: OLED��ʾ������ 
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

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
u8 OLED_GRAM[OLED_SEG][OLED_PAGE];	 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<OLED_PAGE;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<OLED_SEG;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
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
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
//	u8 i;
	u16 retry=0;			

	if(cmd) OLED_RS_Set();
	else OLED_RS_Clr();		  
	
	OLED_CS_Clr();
	SPI_I2S_SendData(SPI1,dat); //SPI ��������	 	

	//BUG: SPI_I2S_GetFlagStatus����ִ�������⡣SPI�ڵ����ʴ���ʱ������δ������ɣ����Ѿ�ִ����CS��λ������OLED��ʾ�쳣
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>1)break;
	}
	OLED_CS_Set();
 // OLED_RS_Set();   	  	
} 
#endif
	  	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	OLED_Clear_Ram();
 	OLED_Refresh_Gram();//������ʾ
}

void OLED_Clear_Ram(void)
{
	u8 i,n;  
	for(i=0;i<OLED_PAGE;i++)for(n=0;n<OLED_SEG;n++)OLED_GRAM[n][i]=0x00; 
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>OLED_MAX_X||y>OLED_MAX_Y)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++)
    {   
		if(size==FONT12)temp=oled_asc2_1206[chr][t];  //����1206����
		else temp=oled_asc2_1608[chr][t];		 //����1608���� 	                          
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
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
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

//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
//mode: 0�����ԣ�1:����
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
//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PC,D,G�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );	//SPI1ʱ��ʹ�� 	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;				 //RST  RS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);						 //�����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;				 //RST  RS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //PA5/6/7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);		 //�����

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
	SPI_SSOutputCmd(SPI1,ENABLE); //ʹ��SPI NSS�ܽ�,
//SPI_I2S_ITConfig(SPI1,DISABLE);
  							  
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ

	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��

	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X1F,OLED_CMD); //Ĭ��0X3F(1/64) 

	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�

	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;

	OLED_WR_Byte(0xC8,OLED_CMD); //*COM������� C8

	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x02,OLED_CMD); //[5:4]����

	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0x8F,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)

	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xF1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;

	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x40,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();
}  

//=======================================
/*-----------------------------
����һ��16*16ͼƬ
����:��ʾ��x,y��ͼƬ���ݵ�ַ
------------------------------*/
void OLED_DrawImg16X16(u8 Start_X,u8 Start_Y,const u8 *pic)
{
	u8 cX,cY;
	cY = 7-Start_Y;
	for (cX=0;cX<16;cX++)  //��һ��
	{
		OLED_GRAM[(Start_X+cX)%128][cY] = *pic++;
	}

	cY -= 1;
	for (cX=0;cX<16;cX++) //�ڶ���
	{
		OLED_GRAM[(Start_X+cX)%128][cY] = *pic++;
	}
}

/*-----------------------------
����һ��24*24ͼƬ
����:��ʾ��x,y��ͼƬ���ݵ�ַ
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
����һ��128*64ͼƬ 
����:ͼƬ���ݵ�ַ
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
����һ��128*16ͼƬ 
����:ͼƬ���ݵ�ַ
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

//����ָ������
//x=0~127
//y=0~1
void OLED_FLIP(u8 x,u8 y,u8 len)
{
	u8 i;
	if (len >= OLED_SEG)len = OLED_SEG-1;
	y = (OLED_PAGE-1)-y*2; //һ����ʾ2��

	for(i=0;i<len;i++)
	{
		OLED_GRAM[x+i][y] = ~OLED_GRAM[x+i][y];
		OLED_GRAM[x+i][y-1] = ~OLED_GRAM[x+i][y-1];
	}
}

/*---------end of file---------*/
