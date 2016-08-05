#include <windows.h>
#include "winapihook.h"
#include "sifilemgr.h"

BOOL g_isHooked = FALSE;

static void HookSI(void)
{
    if (g_isHooked) {
        HookWinApi();
    }
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
            DisableThreadLibraryCalls(hinstDLL);
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

#ifdef _WIN64
#pragma comment(linker, "/EXPORT:VoidExport,@1,NONAME")
#else
#pragma comment(linker, "/EXPORT:_VoidExport,@1,NONAME")
#endif

VOID VoidExport(){}