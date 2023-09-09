/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/gta_misc.cpp
 *  DESCRIPTION:    GTA misc functions
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "gta_misc.hpp"

FILE* CFileMgr::Open(const char* filename, const char* mode) {
    return ((FILE * (__cdecl*)(const char*, const char*))(0x538900))(filename, mode);
}

int32_t CFileMgr::Close(FILE* f) {
    return ((int32_t(__cdecl*)(FILE*))(0x5389D0))(f);
}

int32_t CFileMgr::Read(FILE* f, void* buf, size_t len) {
    return ((int32_t(__cdecl*)(FILE*, void*, size_t))(0x538950))(f, buf, len);
}
