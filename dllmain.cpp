// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dinput8.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)load, hModule, NULL, NULL);
        break;
    case DLL_PROCESS_DETACH:
        FreeLibrary(dinput8_dll);
        break;
    }
    return TRUE;
}

