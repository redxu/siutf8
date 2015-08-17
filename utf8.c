#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "utf8.h"

void utf8_to_gbk(const char* u8,char* gbk,DWORD* gbksize)
{
	if(u8 == NULL)
	{
		return;
	}
	//u8->unicode
	size_t size = MultiByteToWideChar(CP_UTF8,0,u8,-1,NULL,0);
	wchar_t* wansi = (wchar_t *)malloc((size)*sizeof(wchar_t));
	memset(wansi,0,(size)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8,0,u8,-1,wansi,size);
		
	//unicode->ascii
	size = WideCharToMultiByte(CP_ACP,0,wansi,-1,NULL,0,NULL,NULL);
	char* ansi = (char *)malloc(size);
	memset(ansi,0,size);
	WideCharToMultiByte(CP_ACP,0,wansi,-1,ansi,size,NULL,NULL);

	memcpy(gbk,ansi,size);
	*gbksize = size;
	
	//clear	
	free(wansi);
	free(ansi);	
}

void gbk_to_utf8(const char* gbk,char* u8,DWORD* u8size)
{
	if(gbk == NULL)
	{
		return;
	}

	//gbk->unicode
	size_t size = MultiByteToWideChar(CP_ACP,0,gbk,-1,NULL,0);
	wchar_t* wansi = (wchar_t *)malloc((size)*sizeof(wchar_t));
	memset(wansi,0,(size)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP,0,gbk,-1,wansi,size);
	
	//unicode->ascii
	size = WideCharToMultiByte(CP_UTF8,0,wansi,-1,NULL,0,NULL,NULL);
	char* ansi = (char *)malloc(size);
	memset(ansi,0,size);
	WideCharToMultiByte(CP_UTF8,0,wansi,-1,ansi,size,NULL,NULL);
	
	memcpy(u8,ansi,size);
	*u8size = size;

	//clear
	free(wansi);
	free(ansi);
}

//2 utf8 dom
//1 utf8
//0 not utf8
int IsUtf8(char* buf,int size)
{
	int isu8;
	unsigned char* pstart = (unsigned char*)buf;
	unsigned char* pend = (unsigned char*)buf+size;
	const char BOM[3] = {0xef,0xbb,0xbf};
	unsigned char current_byte;
	unsigned char previous_byte;
	int good,bad;
	good = bad = 0;
	
	if(size == 0)
	{
		return 0;
	}
	if(strncmp(buf,BOM,3) == 0)
	{
		return 2;
	}

	current_byte = previous_byte = *pstart;
	while(pstart < pend)
	{
		current_byte = *pstart;
		if((current_byte & 0xc0) == 0x80)
		{
			if((previous_byte & 0xc0) == 0xc0)
			{
				good++;
			}
			else if((previous_byte & 0x80) == 0x00)
			{
				bad++;
			}
		}
		else if((previous_byte & 0xc0) == 0xc0)
		{
			bad++;
		}
		
		previous_byte = current_byte;
		pstart++;
	}
	
	if(good > bad)
	{
		isu8 = 1;
	}
	else if(good == bad)
	{
		//pure ascii
		isu8 = 0;
	}
	else
	{
		isu8 = 0;
	}

	return isu8;
}

