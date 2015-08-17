#ifndef __LDASM__H__
#define __LDASM__H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned long __fastcall SizeOfCode(void *Code, unsigned char **pOpcode);

unsigned long __fastcall SizeOfProc(void *Proc);

char __fastcall IsRelativeCmd(unsigned char *pOpcode);

#ifdef __cplusplus
}
#endif

#endif