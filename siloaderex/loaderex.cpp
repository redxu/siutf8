//////////////////////////////////////////////////////////////////////////////
//
//  Mod From DetourCreateProcessWithDll function (withdll.cpp setdll.cpp)
//
//  Microsoft Research Detours Package, Version 3.0.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
#include <stdio.h>
#include <windows.h>
#include <detours.h>

CHAR szDllPath[1024];
CHAR szCommand[2048];
CHAR szPath[1024];

//////////////////////////////////////////////////////////////////////////////
//
static BOOL CALLBACK ListBywayCallback(PVOID pContext,
                                       PCHAR pszFile,
                                       PCHAR *ppszOutFile)
{
    (void)pContext;

    *ppszOutFile = pszFile;
    if (pszFile) {
        printf("    %s\n", pszFile);
    }
    return TRUE;
}

static BOOL CALLBACK ListFileCallback(PVOID pContext,
                                      PCHAR pszOrigFile,
                                      PCHAR pszFile,
                                      PCHAR *ppszOutFile)
{
    (void)pContext;

    *ppszOutFile = pszFile;
    printf("    %s -> %s\n", pszOrigFile, pszFile);
    return TRUE;
}

static BOOL CALLBACK AddBywayCallback(PVOID pContext,
                                      PCHAR pszFile,
                                      PCHAR *ppszOutFile)
{
    PBOOL pbAddedDll = (PBOOL)pContext;
    if (!pszFile && !*pbAddedDll) {                     // Add new byway.
        *pbAddedDll = TRUE;
        *ppszOutFile = szDllPath;
    }
    return TRUE;
}

BOOL SetFile(PCHAR pszPath,BOOL s_fRemove)
{
    BOOL bGood = TRUE;
    HANDLE hOld = INVALID_HANDLE_VALUE;
    HANDLE hNew = INVALID_HANDLE_VALUE;
    PDETOUR_BINARY pBinary = NULL;

    CHAR szOrg[MAX_PATH];
    CHAR szNew[MAX_PATH];
    CHAR szOld[MAX_PATH];

    szOld[0] = '\0';
    szNew[0] = '\0';

#ifdef _CRT_INSECURE_DEPRECATE
    strcpy_s(szOrg, sizeof(szOrg), pszPath);
    strcpy_s(szNew, sizeof(szNew), szOrg);
    strcat_s(szNew, sizeof(szNew), "#");
    strcpy_s(szOld, sizeof(szOld), szOrg);
    strcat_s(szOld, sizeof(szOld), "~");
#else
    strcpy(szOrg, pszPath);
    strcpy(szNew, szOrg);
    strcat(szNew, "#");
    strcpy(szOld, szOrg);
    strcat(szOld, "~");
#endif
    printf("  %s:\n", pszPath);

    hOld = CreateFile(szOrg,
                      GENERIC_READ,
                      FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL);

    if (hOld == INVALID_HANDLE_VALUE) {
        printf("Couldn't open input file: %s, error: %d\n",
               szOrg, GetLastError());
        bGood = FALSE;
        goto end;
    }

    hNew = CreateFile(szNew,
                      GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hNew == INVALID_HANDLE_VALUE) {
        printf("Couldn't open output file: %s, error: %d\n",
               szNew, GetLastError());
        bGood = FALSE;
        goto end;
    }

    if ((pBinary = DetourBinaryOpen(hOld)) == NULL) {
        printf("DetourBinaryOpen failed: %d\n", GetLastError());
        goto end;
    }

    if (hOld != INVALID_HANDLE_VALUE) {
        CloseHandle(hOld);
        hOld = INVALID_HANDLE_VALUE;
    }

    {
        BOOL bAddedDll = FALSE;

        DetourBinaryResetImports(pBinary);

        if (!s_fRemove) {
            if (!DetourBinaryEditImports(pBinary,
                                         &bAddedDll,
                                         AddBywayCallback, NULL, NULL, NULL)) {
                printf("DetourBinaryEditImports failed: %d\n", GetLastError());
            }
        }

        if (!DetourBinaryEditImports(pBinary, NULL,
                                     ListBywayCallback, ListFileCallback,
                                     NULL, NULL)) {

            printf("DetourBinaryEditImports failed: %d\n", GetLastError());
        }

        if (!DetourBinaryWrite(pBinary, hNew)) {
            printf("DetourBinaryWrite failed: %d\n", GetLastError());
            bGood = FALSE;
        }

        DetourBinaryClose(pBinary);
        pBinary = NULL;

        if (hNew != INVALID_HANDLE_VALUE) {
            CloseHandle(hNew);
            hNew = INVALID_HANDLE_VALUE;
        }

        if (bGood) {
            if (!DeleteFile(szOld)) {
                DWORD dwError = GetLastError();
                if (dwError != ERROR_FILE_NOT_FOUND) {
                    printf("Warning: Couldn't delete %s: %d\n", szOld, dwError);
                    bGood = FALSE;
                }
            }
            if (!MoveFile(szOrg, szOld)) {
                printf("Error: Couldn't back up %s to %s: %d\n",
                       szOrg, szOld, GetLastError());
                bGood = FALSE;
            }
            if (!MoveFile(szNew, szOrg)) {
                printf("Error: Couldn't install %s as %s: %d\n",
                       szNew, szOrg, GetLastError());
                bGood = FALSE;
            }
        }

        DeleteFile(szNew);
    }


  end:
    if (pBinary) {
        DetourBinaryClose(pBinary);
        pBinary = NULL;
    }
    if (hNew != INVALID_HANDLE_VALUE) {
        CloseHandle(hNew);
        hNew = INVALID_HANDLE_VALUE;
    }
    if (hOld != INVALID_HANDLE_VALUE) {
        CloseHandle(hOld);
        hOld = INVALID_HANDLE_VALUE;
    }
    return bGood;
}

//////////////////////////////////////////////////////////////////////// main.
//
int CDECL main(int argc, char **argv)
{
    int idx;
    memset(szPath,0,sizeof(szPath));
    GetModuleFileName(NULL,szPath,sizeof(szPath));
    idx = strlen(szPath)-1;
    while(idx >= 0 && szPath[idx] != '\\')
    {
        szPath[idx--] = '\0';
    }

    sprintf_s(szDllPath,ARRAYSIZE(szDllPath),"%s%s",szPath,"loaderdll.dll");
    sprintf_s(szCommand,ARRAYSIZE(szCommand),"%s%s",szPath,"Insight3.Exe");


    if(argc == 2)
    {
        BOOL f_install = FALSE;
        if(strcmp(argv[1],"/install") == 0)
        {
            f_install = SetFile(szCommand,FALSE);
            if(f_install == TRUE)
            {
                MessageBox(NULL, "Mod ExE Success!","Congratulations", MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(NULL, "Mod ExE Failed!","Error", MB_ICONERROR);
            }
            return 1;
        }
        else if(strcmp(argv[1],"/uninstall") == 0)
        {
            f_install = SetFile(szCommand,TRUE);
            if(f_install == TRUE)
            {
                MessageBox(NULL, "UnMod ExE Success!","Congratulations", MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(NULL, "UnMod ExE Failed!","Error", MB_ICONERROR);
            }
            return 1;
        }
        //case args
        else
        {
        	sprintf_s(szCommand,ARRAYSIZE(szCommand),"%s%s%s",szPath,"Insight3.Exe -p ",argv[1]);
        }
    }


    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

    SetLastError(0);

    if (!DetourCreateProcessWithDll(NULL, szCommand,
                                    NULL, NULL, TRUE, dwFlags, NULL, NULL,
                                    &si, &pi, szDllPath, NULL)) {
        DWORD dwError = GetLastError();
        printf("loaderex.exe: DetourCreateProcessWithDll failed: %d\n", dwError);
        if (dwError == ERROR_INVALID_HANDLE) {
#if DETOURS_64BIT
            printf("loaderex.exe: Can't detour a 32-bit target process from a 64-bit parent process.\n");
#else
            printf("loaderex.exe: Can't detour a 64-bit target process from a 32-bit parent process.\n");
#endif
        }
        ExitProcess(9009);
    }

    ResumeThread(pi.hThread);

    DWORD dwResult = 0;

    return dwResult;
}
//
///////////////////////////////////////////////////////////////// End of File.
