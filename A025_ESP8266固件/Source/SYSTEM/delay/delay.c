#include "delay.h"
#include "sys.h"
#include "rtl.h"

static u8  fac_us=0;//us��ʱ������


//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
	SysTick->LOAD=fac_us*2000; //ʱ����� 2ms��ʱ	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����	
}	


//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload;
	
	told=SysTick->VAL;      //�ս���ʱ�ļ�����ֵ
	reload=SysTick->LOAD;	//LOAD��ֵ	   
	ticks=(nus-1)*fac_us; 		//��Ҫ�Ľ�����	  		 
	tcnt=0;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}; 									    
}

void delay_ms(u16 nms)
{
	while(nms--)
	{
		delay_us(1000);//1000
	}
}


U16 bTmr[USER_TMR_NUMBER];

//����ϵͳ��ʱ����ʱ,��С��λΪ10ms ��1��ϵͳ���ڣ�
//tmr:��ʱ����
//nus:��ʱʱ��
void OS_delay_10ms(u16 tmr, u16 nus)
{
	u8 n;
	n = tmr -1;
	bTmr[n] = 0;
	os_tmr_create(nus,tmr); //������ʱ��
	while(!bTmr[n]);  //bTmr�� os_tmr_call()�����б���1
	return;
}
/*
void TestDly()
{
	while(1)
	{
		LED0 = ~LED0;
		OS_delay_us(100);
		LED0 =~LED0;
		OS_delay_us(100);
	}

}
*/















