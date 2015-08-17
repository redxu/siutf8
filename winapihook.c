#include "winapihook.h"
#include "hook/hookapi.h"
#include "utils.h"
#include "utf8.h"
#include "sifilemgr.h"
#include "sihandlemgr.h"


typedef HANDLE (WINAPI *CreateFileFn)(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);

typedef BOOL (WINAPI *CloseHandleFn)(
	HANDLE hObject
);

typedef BOOL (WINAPI *SetEndOfFileFn)(
	HANDLE hFile
);


CreateFileFn OrgCreateFile = NULL;
CloseHandleFn OrgCloseHandle = NULL;
SetEndOfFileFn OrgSetEndOfFile = NULL;




HANDLE WINAPI HookCreateFile(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	HANDLE handle;
	int u8flag = 0;
	char hookfilename[256];
	struct SiFileInfo* si_file_info = NULL;
	unsigned long hash = HashString(lpFileName);
	
	memset(hookfilename,0,sizeof(hookfilename));
	strcpy(hookfilename,lpFileName);	
	si_file_info = FindSiFileFromLink(hash);	
	if(si_file_info == NULL)
	{
		HANDLE hFile = OrgCreateFile(lpFileName,
								GENERIC_READ,
    							FILE_SHARE_READ,
    							NULL,
    							OPEN_EXISTING,
    							FILE_ATTRIBUTE_NORMAL,
    							NULL);
    	if(hFile == INVALID_HANDLE_VALUE)
    	{
    		OutputDebugStringEx("Function :%s OrgCreateFile %s Failed[%d]",__FUNCTION__,lpFileName,GetLastError());
    		goto RECOVER;
    	}    	
    	DWORD fread;
    	DWORD fsize = GetFileSize(hFile,NULL);
    	char* buffer = (char*)malloc(fsize+1);
    	memset(buffer,0,fsize+1);
    	ReadFile(hFile,buffer,fsize,&fread,NULL);
    	OrgCloseHandle(hFile);	
    	u8flag = IsUtf8(buffer,fsize); 	
	    	
    	//convert
    	if(u8flag != 0)
    	{    		
    		DWORD gbksize = 0;
    		DWORD gbkwriten;
    		char* gbk = (char *)malloc(fsize+1);
			if(u8flag == 1)   		
    			utf8_to_gbk(buffer,gbk,&gbksize);
    		else if(u8flag == 2)
    			utf8_to_gbk(buffer+3,gbk,&gbksize);  		
    		//sprintf(hookfilename,"%s.gbk",lpFileName);
    		GetTmpFilename(hash,hookfilename);
    		HANDLE hGbk = OrgCreateFile(hookfilename,
								GENERIC_WRITE,
    							0,
    							NULL,
    							CREATE_ALWAYS,
    							FILE_ATTRIBUTE_NORMAL,
    							NULL);    							
    		if(hGbk != INVALID_HANDLE_VALUE)
    		{
    			WriteFile(hGbk,gbk,gbksize-1,&gbkwriten,NULL);
    			OrgCloseHandle(hGbk);
    		}
    		else 
    		{
    			OutputDebugStringEx("CreateFile %s Failed![Error=%ld]",hookfilename,GetLastError());
    		}   		
    		free(gbk);	    		
    	}
    	  	
    	free(buffer);
		SiFile_Add(hash,u8flag,(char *)lpFileName,hookfilename);	
	}
	else 
	{		
		u8flag = si_file_info->u8flag;
		strcpy(hookfilename,si_file_info->gbkfile);
	}
	
RECOVER:
	handle = OrgCreateFile(hookfilename,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
									dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	if(u8flag != 0)
	{
		SiHandle_Add(handle,u8flag,(char *)lpFileName,hookfilename);		
	}
	
	return handle;
}



BOOL WINAPI HookCloseHandle(
	HANDLE hObject
)
{
	BOOL rtv;

	SiHandle_Del(hObject);
	rtv = OrgCloseHandle(hObject);
	
	return rtv;
}

BOOL WINAPI HookSetEndOfFile(
	HANDLE hFile
)
{
	BOOL rtv;
	
	rtv = OrgSetEndOfFile(hFile);
	
	struct SiHandleInfo* si_handle_info = NULL;
	si_handle_info = FindSiHandleFromLink(hFile);
	if(si_handle_info != NULL)
	{
		//读文件
		DWORD fread;
		DWORD fsize = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
		char* gbk = (char *)malloc(fsize+1);
		memset(gbk,0,fsize+1);
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		ReadFile(hFile,gbk,fsize,&fread,NULL);
		SetFilePointer(hFile,fsize,NULL,FILE_BEGIN);
    	
    	//转成utf8
    	DWORD utf8size = 0;
    	DWORD utf8writen;
    	char* utf8 = (char *)malloc(2*fsize+3);
    	memset(utf8,0,2*fsize+3);
    	if(si_handle_info->u8flag == 1)
    		gbk_to_utf8(gbk,utf8,&utf8size);
    	else if(si_handle_info->u8flag == 2)
    	{
    		gbk_to_utf8(gbk,utf8+3,&utf8size);
    		utf8[0] = 0xef;
    		utf8[1] = 0xbb;
    		utf8[2] = 0xbf;
    		utf8size += 3;
    	}
    	else 
    	{
    		OutputDebugStringEx("Function :%s Error HandleInfo!",__FUNCTION__);
    	}
    		
    	//写回utf8
    	HANDLE hUtf8 = OrgCreateFile(si_handle_info->orgfile,
								GENERIC_WRITE,
    							0,
    							NULL,
    							CREATE_ALWAYS,
    							FILE_ATTRIBUTE_NORMAL,
    							NULL);    							
		if(hUtf8 != INVALID_HANDLE_VALUE)
		{
			WriteFile(hUtf8,utf8,utf8size-1,&utf8writen,NULL);
			OrgCloseHandle(hUtf8);
		}
		else 
		{
			OutputDebugStringEx("CreateFile %s Failed![Error=%ld]",si_handle_info->orgfile,GetLastError());
		}
		
		free(utf8);
		free(gbk);
	}
	
	return rtv;
}


BOOL HookWinApi(void)
{
	OrgCreateFile = (CreateFileFn)HookFunction("kernel32.dll","CreateFileA",(void *)HookCreateFile);
	if(OrgCreateFile == NULL)
	{
		OutputDebugString("Hook CreateFile Failed!");
		return FALSE;
	}
	
	OrgCloseHandle = (CloseHandleFn)HookFunction("kernel32.dll","CloseHandle",(void *)HookCloseHandle);
	if(OrgCloseHandle == NULL)
	{
		OutputDebugString("Hook CloseHandle Failed!");
		return FALSE;
	}
	
	OrgSetEndOfFile = (SetEndOfFileFn)HookFunction("kernel32.dll","SetEndOfFile",(void *)HookSetEndOfFile);
	if(OrgSetEndOfFile == NULL)
	{
		OutputDebugString("Hook SetEndOfFile Failed!");
		return FALSE;
	}
	
	return TRUE;
}
