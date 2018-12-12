#include "EC11_Key.h"
#include "stm32f10x_exti.h"
#include "usart.h"
#include "oled.h"
/*------------------------------------
EC11��ת��ť�ӿڳ�ʼ��
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

	//��ʼ��PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//ʹ��PORTAʱ��

	//��ʼ�� EC11_Key-->GPIOA.6	  ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.6

	//EC11_A-->PB10,EC11_B-->PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.6

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10 | GPIO_PinSource11);
	EXTI_InitStructure.EXTI_Line = EXTI_Line10 | EXTI_Line11;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
#else
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//��ʼ�� EC11_Key-->GPIOA.6	  ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.6

	//EC11_A-->PB10,EC11_B-->PB11
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//��GPIO AFIO��ʱ��  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);//PC10  ΪGPIOC��PIN10  
	EXTI_InitStructure.EXTI_Line = EXTI_Line10; //PC11��Ϊ��EXTI_Line11  
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //�жϷ�ʽΪ�������½���  
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
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) //����Ϊ�ж���Ӧ���жϺ��Ƿ�����жϣ�����ж���жϵĻ���  
	{
		EXTI_ClearITPendingBit(EXTI_Line10);       //���ж�  
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



