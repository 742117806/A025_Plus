/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 字符类操作
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "my_string.h"
#include <string.h>
#include <math.h>

//查找字符在字符串中第一个出现的位置
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

//把unsigned char 数值转成十六进制字符
char *strhex(unsigned char n)
{
	static char hex[3];
	hex[0] = tohex(n/16);
	hex[1] = tohex(n%16);
	hex[2] = '\0';
	return hex;
}

//把10进制的字符串转成数值
int strdec(char *s1)
{
	int len;   //
	int num = 0;
	char *p;

	p = s1;
	len = strlen(s1);
	while(len--)
	{
		if (*p >= '0' && *p <= '9')  // 0~9的数
		{
			num = num + pow_u(10,len)* (*p-'0');
			p++;
		}
		else
		{
			num = 0;
			break;  //非法字符，跳出
		}
	}
	return num;
}

//把0~15的数转成十六进制0~F
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

//清除内存
//addr： 地址
//n： 长度
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

//m^n函数
u32 pow_u(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//把数值转换成字符串
char* itostr(u16 num)
{  
	static char str[6];
	u8 len;
	u8 t,temp;

	//计算数值的长度
	temp = 1;
	for (len=1;len<6;len++)
	{
		temp *=10;
		if ((num / temp) == 0)break;  //结果为0，就跳出
	}

	//转换成字符
	for(t=0;t<len;t++)
	{
		temp=num/pow_u(10,len-t-1) % 10;
		str[t] = '0'+temp;		
	}
	str[t] = '\0'; //结束符
	
	return str;
}

//字符串str1中，寻找字串str2，若找到返回找到的位置，否则返回NULL。
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

//在一串字符串中截取两个字符之间的字符串
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

//C语言删除字符串中特定字符 
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


//小写字母转大写
void LowerCharToUpperChar(char a[],int n)  //n是字符串长度 
{
	int i;
	for (i = 0; i<n; i++)
	{
		if (a[i] > 96 && a[i] < 123)
			a[i] -= 32;
	}
}
