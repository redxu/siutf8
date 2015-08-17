#include <string.h>
#include <stdio.h>
#include "utils.h"

//输出调试信息
void WINAPI OutputDebugStringEx(LPCTSTR lpcFormatText, ...)
{
	char szBuffer[1024];

    va_list argptr;
    va_start(argptr, lpcFormatText);
    vsprintf(szBuffer, lpcFormatText, argptr);
    va_end(argptr);

	OutputDebugString(szBuffer);
}


//copy from openssl
//字符串hash
unsigned long HashString(const char* str)
{
	int i,l; 
	unsigned long ret=0; 
	unsigned short *s; 
	
	 
	if(str == NULL) 
		return(0); 
	l=(strlen(str)+1)/2; 
	s=(unsigned short *)str; 
	for (i=0; i<l;i++)
		ret^=(s[i]<<(i&0x0f)); 
	return(ret); 	
}

static void GetFilenameFromPath(const char* path,char* filename)
{
	int len = strlen(path);
	int i;
	for(i=len-1;i>=0;i--)
	{
		if(path[i] == '\\')
		{
			strcpy(filename,&path[i+1]);
			return;
		}
	}
	
	strcpy(filename,path);
}

/*
void GetTmpFilename(const char* orgfile,char* newfile)
{
	char tmppath[256];
	char filename[256];
	GetFilenameFromPath(orgfile,filename);
	GetTempPath(sizeof(tmppath),tmppath);
	sprintf(newfile,"%s\\%s.gbk",tmppath,filename);
}
*/

void GetTmpFilename(unsigned long hash,char* newfile)
{
	char tmppath[256];
	GetTempPath(sizeof(tmppath),tmppath);
	sprintf(newfile,"%s\\%lu.gbk",tmppath,hash);	
}

