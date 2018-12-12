#include "stm32f10x.h"
#include "KEY_UPDATA.h"
//#include "sys.h" 
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Updata_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化PB2  强制升级按键
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//使能PORTB时钟

	//初始化 updata-->GPIOB.2	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.2

	//初始化 updata-->GPIOB.2	  下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认上拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.2

	//初始化 updata-->GPIOB.2	  下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认上拉	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOB.2
}

void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);        //JTAG-DP 失能 + SW-DP使能 

	GPIO_InitStructure.GPIO_Pin = Gpio_Beep_pin;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_ResetBits(GPIOC,Gpio_Beep_pin);						 //PB.5 输出高

}
