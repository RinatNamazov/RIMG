/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           misc/keys.hpp
 *  DESCRIPTION:    Encryption keys
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

uint8_t hdr_key[32] = {'R',  0x17, 0xFF, '%',  0x04, '7',  0x06, 'K',  0x08, '3',  0x0A,
                       0x5D, 0xB6, '0',  0x0E, 0x0F, '1',  0x11, 0x6F, 0x5F, 0xF1, 0x8F,
                       0x48, 0xAB, 0x18, 0xEF, 0xFF, 0x1B, 0x1C, 0x1D, 0x03, 'W'};

uint8_t elm_key[32]
    = {'W', 0x03, 'H', 'G', 0x04, '$',  0x06, 'K',  0x08, '3', 'S',  0x5D, 0xB6, '0',  'K',  0x0F,
       '1', 0x11, 'W', 'C', 0xF1, 0x8F, 'M',  0xAB, 0x18, 'K', 0xFF, 0x1B, 0x1C, 0x1D, 0x17, 'R'};

uint8_t met_key[32]
    = {'U', 0x03, 'g', 'b', 0x02, '/',  0x66, ';',  0x68, 'm', 'S',  0x2C, 0x6F, '0',  '4',  0xD2,
       'c', 0x11, 'W', 'C', 0xF1, 0x8F, 'M',  0xF5, 0xB4, 'K', 0xFF, 0x1B, 0x1C, 0x2D, 0x27, 'g'};

uint8_t iv[16] = {'R', 'I', 'N', 'A', 'T', '_', 'N', 'A', 'M', 'A', 'Z', 'O', 'V', 0x17, 'R', 'W'};
