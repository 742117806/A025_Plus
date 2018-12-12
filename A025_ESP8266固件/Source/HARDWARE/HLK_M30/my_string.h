/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ºý¶Á³æ
@Version : 1.0
@Date    : 2015-8-12
@Description: ×Ö·ûÀà²Ù×÷
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __MY_STRING_
#define __MY_STRING_
#include "sys.h"

int strpos(char* s1,char ch);
int strdec(char *s1);
char tohex(unsigned char n);
char *strhex(unsigned char n);
void ClrMem(void *p,int n);
u32 pow_u(u8 m,u8 n);
char* itostr(u16 num);

const char* StrStr(const char *str1, const char *str2);
void StrObtainStr(u8 *str_Source, u8 *str_Dest, char index_c1, char index_c2);
void Del_char(char str[], char c);
void LowerCharToUpperChar(char a[], int n); //nÊÇ×Ö·û´®³¤¶È
#endif

