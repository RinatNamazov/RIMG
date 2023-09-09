/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/utils.hpp
 *  DESCRIPTION:    Utils
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

void redirect_code(uintptr_t address, void* to_address, bool is_ñall = false);
void patch_set_nop(uintptr_t address, size_t size);
void patch_byte(uintptr_t address, uint8_t value);
void patch_pointer(uintptr_t address, uintptr_t value);
