#ifndef __UTF8__H__
#define __UTF8__H__

//utf8תgbk
void utf8_to_gbk(const char* u8,char* gbk,DWORD* gbksize);

//gbkתutf8
void gbk_to_utf8(const char* gbk,char* u8,DWORD* u8size);

//2 utf8 dom
//1 utf8
//0 not utf8
int IsUtf8(char* buf,int size);


#endif
