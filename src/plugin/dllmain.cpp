/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/dllmain.cpp
 *  DESCRIPTION:    DllMain
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "dllmain.hpp"

#include <Windows.h>

#include "Plugin.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) {
    static Plugin* plugin;

    switch (dwReasonForCall) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            plugin = new Plugin();
            break;

        case DLL_PROCESS_DETACH:
            if (plugin != nullptr) {
                delete plugin;
            }
            break;
    }

    return TRUE;
}
