#include "delay.h"
#include "sys.h"
#include "rtl.h"

static u8  fac_us=0;//us延时倍乘数


//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  
	SysTick->LOAD=fac_us*2000; //时间加载 2ms定时	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	
}	


//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload;
	
	told=SysTick->VAL;      //刚进入时的计数器值
	reload=SysTick->LOAD;	//LOAD的值	   
	ticks=(nus-1)*fac_us; 		//需要的节拍数	  		 
	tcnt=0;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
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

//采用系统定时器延时,最小单位为10ms （1个系统周期）
//tmr:定时器号
//nus:延时时间
void OS_delay_10ms(u16 tmr, u16 nus)
{
	u8 n;
	n = tmr -1;
	bTmr[n] = 0;
	os_tmr_create(nus,tmr); //创建定时器
	while(!bTmr[n]);  //bTmr在 os_tmr_call()函数中被置1
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















