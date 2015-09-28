#include "sihandlemgr.h"
#include "utils.h"
#include <string.h>




//句柄链表
typedef struct SiHandleLink
{
	struct SiHandleInfo handleinfo;
	struct SiHandleLink* next;
};

static struct SiHandleLink* si_handle_link = NULL;

//创建一个SiHandleLink结构
static struct SiHandleLink* SiHandle_Create(HANDLE handle,int u8flag,char* orgfile,char* gbkfile)
{
	struct SiHandleLink* tmp = (struct SiHandleLink*)malloc(sizeof(struct SiHandleLink));
	memset(tmp,0,sizeof(struct SiHandleLink));
	tmp->handleinfo.handle = handle;
	tmp->handleinfo.u8flag = u8flag;
	strcpy(tmp->handleinfo.orgfile,orgfile);
	strcpy(tmp->handleinfo.gbkfile,gbkfile);
	return tmp;
}

static void SiHandle_Destory(struct SiHandleLink* node)
{
	free(node);
}

//添加到链表
void SiHandle_Add(HANDLE handle,int u8flag,char* orgfile,char* gbkfile)
{
	if(handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	struct SiHandleLink* file = SiHandle_Create(handle,u8flag,orgfile,gbkfile);
	struct SiHandleLink* tmp = si_handle_link;
	
	if(tmp == NULL)
	{
		si_handle_link = file;
		return;
	}
	
	while(tmp->next)
	{
		tmp = tmp->next;
	}
	tmp->next = file;
	
	return;
}


//从链表删除
void SiHandle_Del(HANDLE handle)
{
	struct SiHandleLink* cur = si_handle_link;
	struct SiHandleLink* prev = NULL;
	
	while(cur)
	{
		if(cur->handleinfo.handle == handle)
		{
			if(cur == si_handle_link)
			{
				si_handle_link = si_handle_link->next;
			}
			else 
			{
				prev->next = cur->next;
			}
			SiHandle_Destory(cur);
			return;
		}
		
		prev = cur;
		cur = cur->next;
	}	
	return;
}

//调试
void SiHandle_Debug(void)
{
	struct SiHandleLink* cur = si_handle_link;
	int size = 0;
	if(cur == NULL)
	{
		OutputDebugString("SiHandleLink size=0");
		return;
	}
	while(cur)
	{
		size++;
		cur = cur->next;
	}
	OutputDebugStringEx("SiHandleLink size=%d",size);
}

//从链表中查找句柄
struct SiHandleInfo* FindSiHandleFromLink(HANDLE handle)
{
	struct SiHandleLink* cur = si_handle_link;
	
	while(cur)
	{
		if(cur->handleinfo.handle == handle)
			return &cur->handleinfo;
			
		cur = cur->next;
	}	
	
	return NULL;
}
