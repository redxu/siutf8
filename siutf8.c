#include <windows.h>
#include "winapihook.h"
#include "sifilemgr.h"

static void HookSI(void)
{
	HookWinApi();
}

static void UnhookSI(void)
{
	SiFile_Unlink();
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            HookSI();
            break;

        case DLL_PROCESS_DETACH:
            UnhookSI();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE; // succesful
}

