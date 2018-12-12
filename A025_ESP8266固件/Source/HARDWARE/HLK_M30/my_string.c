/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �ַ������
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "my_string.h"
#include <string.h>
#include <math.h>

//�����ַ����ַ����е�һ�����ֵ�λ��
int strpos(char* s1,char ch)
{
	int pos = 0;
	char *p;

	p = s1;
	while(*p)
	{
		if (*p == ch)
		{
			return pos;
		}
		pos++;
		p++;
	}
	return -1;
}

//��unsigned char ��ֵת��ʮ�������ַ�
char *strhex(unsigned char n)
{
	static char hex[3];
	hex[0] = tohex(n/16);
	hex[1] = tohex(n%16);
	hex[2] = '\0';
	return hex;
}

//��10���Ƶ��ַ���ת����ֵ
int strdec(char *s1)
{
	int len;   //
	int num = 0;
	char *p;

	p = s1;
	len = strlen(s1);
	while(len--)
	{
		if (*p >= '0' && *p <= '9')  // 0~9����
		{
			num = num + pow_u(10,len)* (*p-'0');
			p++;
		}
		else
		{
			num = 0;
			break;  //�Ƿ��ַ�������
		}
	}
	return num;
}

//��0~15����ת��ʮ������0~F
char tohex(unsigned char n)
{
	char ch;
	if (n < 10) //0~9
	{
		ch = '0'+n;
	}
	else if (n < 16)
	{
		ch = 'A'+ (n-10);
	}
	else 
	{
		ch = 'F';
	}
	return ch;
}

//����ڴ�
//addr�� ��ַ
//n�� ����
void ClrMem(void *addr,int n)
{
	char *p;
	int t;
	p = (char *)addr;
	if (n>0)
	{
		t=n;
		while(t--)
		{
			*p++ = 0;
		}
	}
}

//m^n����
u32 pow_u(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//����ֵת�����ַ���
char* itostr(u16 num)
{  
	static char str[6];
	u8 len;
	u8 t,temp;

	//������ֵ�ĳ���
	temp = 1;
	for (len=1;len<6;len++)
	{
		temp *=10;
		if ((num / temp) == 0)break;  //���Ϊ0��������
	}

	//ת�����ַ�
	for(t=0;t<len;t++)
	{
		temp=num/pow_u(10,len-t-1) % 10;
		str[t] = '0'+temp;		
	}
	str[t] = '\0'; //������
	
	return str;
}

//�ַ���str1�У�Ѱ���ִ�str2�����ҵ������ҵ���λ�ã����򷵻�NULL��
const char* StrStr(const char *str1, const char *str2)
{
	if ((NULL != str1) && (NULL != str2))
	{
		while (*str1 != '\0')
		{
			const char *p = str1;
			const char *q = str2;
			const char *res = NULL;
			if (*p == *q)
			{
				res = p;
				while (*p && *q && *p++ == *q++)
					;

				if (*q == '\0')
					return res;
			}
			str1++;
		}
	}
	return NULL;
}

//��һ���ַ����н�ȡ�����ַ�֮����ַ���
void StrObtainStr(u8 *str_Source, u8 *str_Dest, char index_c1, char index_c2)
{
	u8 *p1;
	u8 *p2;
	u8 len;

	p1 = (u8*)strchr((const char*)str_Source, index_c1);
	p2 = (u8*)strchr((const char*)p1+1, index_c2);

	if ((p1 != NULL) && (p2 != NULL))
	{
		p1 = p1 + 1;
		len = p2 - p1;
		memcpy(str_Dest, p1, len);
	}
}

//C����ɾ���ַ������ض��ַ� 
void Del_char(char str[], char c)
{
	int i = 0, j = 0;
	while (str[i] != '\0')
	{
		if (str[i] != c)
		{
			str[j++] = str[i++];
		}
		else
		{
			i++;
		}
	}
	str[j] = '\0';
}


//Сд��ĸת��д
void LowerCharToUpperChar(char a[],int n)  //n���ַ������� 
{
	int i;
	for (i = 0; i<n; i++)
	{
		if (a[i] > 96 && a[i] < 123)
			a[i] -= 32;
	}
}
