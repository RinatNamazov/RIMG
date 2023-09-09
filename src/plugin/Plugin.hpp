/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/Plugin.hpp
 *  DESCRIPTION:    Plugin
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#pragma once

#define NAKED __declspec(naked)

#ifdef _DEBUG
    #define debug_print(str, ...) printf("[RIMG] " str "\n", __VA_ARGS__)
#else
    #define debug_print
#endif

class Plugin final {
public:
    Plugin();
    ~Plugin();
};
