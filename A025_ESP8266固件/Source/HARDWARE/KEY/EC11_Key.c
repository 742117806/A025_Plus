#include "EC11_Key.h"
#include "stm32f10x_exti.h"
#include "usart.h"
#include "oled.h"
/*------------------------------------
EC11旋转按钮接口初始化
-------------------------------------*/
u8 ec11_LeftCnt = 0;
u8 ec11_RightCnt = 0;
u8 percent_buff[10] = { 0 };
void EC11_KeyInit(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//初始化PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//使能PORTA时钟

	//初始化 EC11_Key-->GPIOA.6	  下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.6

	//EC11_A-->PB10,EC11_B-->PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.6

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10 | GPIO_PinSource11);
	EXTI_InitStructure.EXTI_Line = EXTI_Line10 | EXTI_Line11;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键KEY2所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
#else
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//初始化 EC11_Key-->GPIOA.6	  下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.6

	//EC11_A-->PB10,EC11_B-->PB11
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//打开GPIO AFIO的时钟  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);//PC10  为GPIOC的PIN10  
	EXTI_InitStructure.EXTI_Line = EXTI_Line10; //PC11，为：EXTI_Line11  
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //中断方式为上升与下降沿  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   //NVIC  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

void EXTI15_10_IRQHandler(void)
{
	//EC11_A
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) //这里为判断相应的中断号是否进入中断，如果有多个中断的话。  
	{
		EXTI_ClearITPendingBit(EXTI_Line10);       //清中断  
		if (EC11_B == 1)
		{
			//printf("Right\r\n");
			ec11_RightCnt = 1;
		}
		else
		{
			//printf("Left\r\n");
			ec11_LeftCnt=1;
		}
		//sprintf(percent_buff, "EC11=%%%d  ",ec11_LeftCnt);
		//OLED_ShowString(0, 0, percent_buff);
		//OLED_Refresh_Gram();
	}
}



