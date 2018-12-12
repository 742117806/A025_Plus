#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stm32f10x.h "

/*-------------��������----------------*/
/*************
1�����ж����� SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET ��ΪSCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET;
2������ѡ��IROM����ʼ��ַ�ǣ�0x8000000 + APP_VECT_TAB_OFFSET
3��SIZE��С��0x10000 - APP_VECT_TAB_OFFSET
4��BootLoader��APP1��ַҲҪ����Ӧ�޸� 
***************/
#define APP_VECT_TAB_OFFSET		0x2800	//0x200*12=0x1800, ��12K�Ŀռ��BOOTLOADER

//STM32 ��ַ
#define FLASH_PAGE				1024
#define FLASH_END_ADDR			0x08010000
#define FLASH_SYS_ADDR			(FLASH_END_ADDR-FLASH_PAGE*1) //�����״�����  
#define FLASH_LANGUAGE_ADDR		(FLASH_SYS_ADDR + 0x02)   //��������
#define FLASH_CUSTOM_CODE		(FLASH_SYS_ADDR + 0x04) //����ͻ���ʶ����
#define FLASH_TOUCH_ADDR		(FLASH_SYS_ADDR + 0x10)  //���津����Ϣ
#define FLASH_MANUAL_PWM		(FLASH_SYS_ADDR + 0x20)  //�����ֶ���Ϣ
#define FLASH_EASY_TIMER		(FLASH_SYS_ADDR + 0x30)  //����EASY������Ϣ
#define FLASH_SYSFLAG_ADDR		(FLASH_SYS_ADDR + 0x50)  //����ϵͳ������Ϣ
#define FLASH_EFFECT_ADDR		(FLASH_SYS_ADDR + 0x60)  //����ƹ�Ч��
#define FLASH_TIMER_ADDR		(FLASH_SYS_ADDR + 0xA0)  //���涨ʱ��Ϣ
//#define FLASH_MOONLIGHT_ADDR	(FLASH_SYS_ADDR + 0xA0+240)  //����������Ϣ
#define FLASH_MOTOR_INFO_ADDR	(FLASH_SYS_ADDR + 0xA0+240)	//��������Ϣ

//#define FLASH_MOONLIGHT_ADDR   (FLASH_SYS_ADDR + 0xA0)  //����������Ϣ
//#define FLASH_TIMER_ADDR   (FLASH_SYS_ADDR + 0x170)  //���涨ʱ��Ϣ

//W25Q64 Flash�ռ����
#define W25QXX_USER_ADDR		0x000FF000	//
#define W25QXX_CUSTOM_CODE		(W25QXX_USER_ADDR+0)//�ͻ����� 16λ
#define W25QXX_PWM_NUM			(W25QXX_USER_ADDR+2)//PWM����
#define W25QXX_MAC				(W25QXX_USER_ADDR+4)//MAC��ַ
#define W25QXX_CHIP_CODE		(W25QXX_USER_ADDR+32)//оƬ����
#define W25QXX_LOGO				(W25QXX_USER_ADDR+64)//LOGO����

#define W25QXX_FONT_ADDR		0x00100000  //�����ַ

//����ģʽ
#define MODE_TIMER		0x00	//��ʱģʽ
#define MODE_MUNUAL		0x01	//�ֶ�ģʽ
//#define MODE_WEATHER	0x02	//���� �Ʋ�//����ģʽ

#define MODE_FLASH		0x02
#define MODE_CLUDE		0x03
#define MODE_FLASH_VEIW	0x04
#define MODE_CLUDE_VEIW	0x05
#define	MODE_MOONLIGHT	0x06	//�¹�ģʽ
#define MODE_MOONLIGHT_VEIW	0x07//�¹�ģʽԤ��
#define MODE_UPDATA		0x80 //����APP

//#define MODE_WEATHER_VIEW	0x07	//���� �Ʋ�//����ģʽ
#define MODE_POWER_OFF		0x08	//�ص�ģʽ
#define MODE_PREVIEW	0x10	//��ʱԤ��

//0x02����   0x03����    0x04����Ԥ��   0x05 ����Ԥ��

//����汾
#define V1	20
#define V2	96
extern const u8 version[2];

#define SUNRISE_TIME	120	//�ճ�������ʱ��,��λ����
#define PWM_SPEED_TIMER		20	//��ʱģʽ��PWM�仯�ٶ�
#define PWM_SPEED_FLASH		30	//��ʱģʽ��PWM�仯�ٶ�
#define PWM_SPEED_DIMMER	1	//�ֶ�ģʽ��PWM�仯�ٶ�	
#define PWM_SPEED_PREVIEW	1	//��ʱԤ����PWM�仯�ٶ�	

//�������� 
#define UPDATA_APP	0  //�������
#define UPDATA_FONT	1  //�����ֿ�

//�ͻ�����,�ͻ�,�漰����LOGO��WIFI����
//#define USER_SANRISE  1	 //1:Sanrise;
//#define USER_AQUALED  1	 //2:Aqualed;
//#define USER_ARCADIA  1	 //3:Arcadia;
//#define USER_SPECTRA	1	//4:Spectra
#define USER_AQUARIUM	1	//5:Spectra
#define LOGO_EN		0  //�Ƿ����ÿ���LOGO


#endif
