#ifndef __UTILS__H__
#define __UTILS__H__

#include <windows.h>

//输出调试信息
void WINAPI OutputDebugStringEx(LPCTSTR lpcFormatText, ...);

//字符串hash
unsigned long HashString(const char* str);

//获取临时文件名
//void GetTmpFilename(const char* orgfile,char* newfile);
void GetTmpFilename(unsigned long hash,char* newfile);

#endif
