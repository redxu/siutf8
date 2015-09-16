/********************************************************************
*  模块名: MD5算法模块
*  摘要:
********************************************************************/

#ifndef __MD5_H__
#define __MD5_H__


#ifndef uint8
	#define uint8  unsigned char
#endif

#ifndef uint32
	#define uint32 unsigned long int
#endif

typedef struct
{
	uint32 total[2];
	uint32 state[4];
	uint8 buffer[64];
}md5_context;


#ifdef  __cplusplus
extern "C" {
#endif


/**************************
功能	:计算数据流的MD5码
输入	:buf－要计算的数据流
		:len－要计算的数据流的长度
输出	:md5sum－生成的MD5码，固定16bytes
返回	:无
注：名字和终端不同，做调整
*/
void Md5Sum(unsigned char *buf, int len, unsigned char md5sum[16]);


#ifdef  __cplusplus
}
#endif


#endif

