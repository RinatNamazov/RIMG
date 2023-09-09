/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/gta_misc.hpp
 *  DESCRIPTION:    GTA misc functions
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#pragma once

#include <Windows.h>

#include <cstdint>
#include <cstdio>

class CFileMgr {
public:
    static FILE*   Open(const char* filename, const char* mode);
    static int32_t Close(FILE* f);
    static int32_t Read(FILE* f, void* buf, size_t len);
};
