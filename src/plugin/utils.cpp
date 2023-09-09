/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/utils.cpp
 *  DESCRIPTION:    Utils
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "utils.hpp"

#include <Windows.h>

void redirect_code(uintptr_t address, void* to_address, bool is_ñall) {
    DWORD old_vp;
    VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &old_vp);
    *reinterpret_cast<BYTE*>(address)      = is_ñall ? 0xE8 /* call */ : 0xE9 /* jmp */;
    *reinterpret_cast<DWORD*>(address + 1) = reinterpret_cast<DWORD>(to_address) - address - 5;
    VirtualProtect(reinterpret_cast<void*>(address), 5, old_vp, &old_vp);
}

void patch_set_nop(uintptr_t address, size_t size) {
    DWORD old_vp;
    VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old_vp);
    memset(reinterpret_cast<void*>(address), 0x90, size);
    VirtualProtect(reinterpret_cast<void*>(address), size, old_vp, &old_vp);
}

void patch_byte(uintptr_t address, uint8_t value) {
    DWORD old_vp;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), PAGE_EXECUTE_READWRITE,
                   &old_vp);
    *reinterpret_cast<uint8_t*>(address) = value;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), old_vp, &old_vp);
}

void patch_pointer(uintptr_t address, uintptr_t value) {
    DWORD old_vp;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), PAGE_EXECUTE_READWRITE,
                   &old_vp);
    *reinterpret_cast<uintptr_t*>(address) = value;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(value), old_vp, &old_vp);
}
