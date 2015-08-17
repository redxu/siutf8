#ifndef __SIHANDLEMGR__H__
#define __SIHANDLEMGR__H__

#include <windows.h>

//句柄结构
typedef struct SiHandleInfo
{
	HANDLE handle;
	int u8flag;
	char orgfile[256];
	char gbkfile[256];
};


//添加到链表
void SiHandle_Add(HANDLE handle,int u8flag,char* orgfile,char* gbkfile);


//从链表删除
void SiHandle_Del(HANDLE handle);

//调试
void SiHandle_Debug(void);

//从链表中查找句柄
struct SiHandleInfo* FindSiHandleFromLink(HANDLE handle);

#endif

