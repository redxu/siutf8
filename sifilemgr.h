#ifndef __SIFILEMGR__H__
#define __SIFILEMGR__H__


//文件结构
typedef struct SiFileInfo
{
	unsigned long hash;
	int u8flag;
	unsigned char orgmd5[16];
	char orgfile[256];
	char gbkfile[256];
};


//添加到链表
void SiFile_Add(unsigned long hash,int u8flag,unsigned char md5[16],char* orgfile,char* gbkfile);


//删除临时文件
void SiFile_Unlink(void);

//从链表中查找文件
struct SiFileInfo* FindSiFileFromLink(unsigned long hash);


#endif
