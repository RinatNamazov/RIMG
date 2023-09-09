/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           misc/types.hpp
 *  DESCRIPTION:    Structs of IMG
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

enum FileFlags : uint32_t {
    FILE_FLAG_NONE       = 0,
    FILE_FLAG_ENCRYPTED  = 1 << 0,
    FILE_FLAG_COMPRESSED = 1 << 2
};

#pragma pack(push, 1)
struct CryptImgHeader {
    uint32_t check; // Constant value 1703, after decryption it should have value 1703. To check if
                    // decryption worked properly.
    uint32_t number_of_entries;
    uint32_t flags;
    char     reserved[4];
};

struct FileMetaInfo {
    uint32_t  file_size;
    FileFlags flags;
    uint8_t   key[32];
    char      reserved[8];
};

struct DirEntry {
    uint32_t offset;
    uint16_t streaming_size;
    uint16_t size_in_archive;
    char     file_name[24];
};
#pragma pack(pop)
