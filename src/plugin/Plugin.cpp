/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           plugin/Plugin.cpp
 *  DESCRIPTION:    Plugin
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "Plugin.hpp"

#include <Windows.h>

#include <cstdio>

#include "aes.hpp"
#include "gta_misc.hpp"
#include "misc/keys.hpp"
#include "misc/types.hpp"
#include "utils.hpp"

// Maybe make local variable?
bool is_encrypted_img;

uint32_t __stdcall patch_CStreaming__LoadCdDirectory_5B61B0_2(FILE* f, char* a) {
    is_encrypted_img = false;
    if (a[0] == 'R' && a[1] == 'I' && a[2] == 'N' && a[3] == 'W') {
        char b[4];
        CFileMgr::Read(f, &b, sizeof(b));
        if (b[0] == 'A' && b[1] == 'R' && b[2] == 'E' && b[3] == 'S') {
            CryptImgHeader crypt_header;
            CFileMgr::Read(f, &crypt_header, sizeof(crypt_header));

            struct AES_ctx hdr_ctx;
            AES_init_ctx_iv(&hdr_ctx, hdr_key, iv);
            AES_CBC_decrypt_buffer(&hdr_ctx, reinterpret_cast<uint8_t*>(&crypt_header),
                                   sizeof(crypt_header));

            if (crypt_header.ñheck == 1703) {
                is_encrypted_img = true;
                return crypt_header.number_of_entries;
            } else {
                debug_print("crypt_header.check != 1703");
            }
        }
    }
    return 0;
}

NAKED void __cdecl patch_CStreaming__LoadCdDirectory_5B61B0() {
    __asm {
        lea ecx, [esp + 0x54 - 0x28]
        push ecx // buff
        push esi // file
        call patch_CStreaming__LoadCdDirectory_5B61B0_2
        cmp eax, 0 // Is no encrypted?
        je no_encrypted
        
        sub esp, 0xC
        mov [esp + 0x60 - 0x38], eax // numberOfEntries
        push 0x5B61C1 // Go back.
        retn

    no_encrypted:
        push 4
        lea edx, [esp + 0x58 - 0x38]
        
        push 0x5B61B6; // Go back.
        retn
    }
}

int32_t __cdecl patch_CStreaming__LoadCdDirectory_5B61E1(FILE* file, void* buf, size_t len) {
    int32_t return_value = CFileMgr::Read(file, buf, len);
    if (is_encrypted_img) {
        struct AES_ctx elm_ctx;
        AES_init_ctx_iv(&elm_ctx, elm_key, iv);
        AES_CBC_decrypt_buffer(&elm_ctx, reinterpret_cast<uint8_t*>(buf), len);
    }
    return return_value;
}

bool __cdecl patch_CStreaming__ConvertBufferToObject_40E2C5(uint8_t* buffer, int32_t model_id) {
    uint8_t* buffer_ptr = buffer;

    if (buffer[0] == 'R' && buffer[1] == 'I' && buffer[2] == 'N' && buffer[3] == 'W'
        && buffer[4] == 'A' && buffer[5] == 'R' && buffer[6] == 'E' && buffer[7] == 'S') {
        buffer += 8;

        uint32_t file_size = *reinterpret_cast<uint32_t*>(buffer);
        buffer += sizeof(file_size);
        debug_print("file_size: %d", file_size);

        struct AES_ctx met_ctx;
        AES_init_ctx_iv(&met_ctx, met_key, iv);
        AES_CBC_decrypt_buffer(&met_ctx, reinterpret_cast<uint8_t*>(buffer), sizeof(FileMetaInfo));

        FileMetaInfo* meta_info = reinterpret_cast<FileMetaInfo*>(buffer);
        buffer += sizeof(FileMetaInfo);
        debug_print("meta_info: file_size: %d, flags: %d", meta_info->file_size, meta_info->flags);

        if (file_size != meta_info->file_size) { // Decrypting failed?
            debug_print("file_size != meta_info.file_size (%d != %d)", file_size,
                        meta_info->file_size);
        } else {
            if (meta_info->flags & FILE_FLAG_COMPRESSED) {
                debug_print("flags: compressed");
                // Todo.
            }
            if (meta_info->flags & FILE_FLAG_ENCRYPTED) {
                debug_print("flags: encrypted");
                struct AES_ctx fle_ctx;
                AES_init_ctx(&fle_ctx, meta_info->key);
                for (uint32_t i = 0; i < (file_size / AES_BLOCKLEN); ++i) {
                    AES_ECB_decrypt(&fle_ctx, buffer + (i * AES_BLOCKLEN));
                }
            }
        }
        memcpy(buffer_ptr, buffer, file_size);
    }

    return ((bool(__cdecl*)(uint8_t*, int32_t))0x40C6B0)(buffer_ptr, model_id);
}

BOOL WINAPI patch_StreamReaderThread_406567(HANDLE hFile, LPVOID lpBuffer,
                                            DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                                            LPOVERLAPPED lpOverlapped) {
    debug_print("Reading sectors: %d", nNumberOfBytesToRead / 2048);
    BOOL return_value
        = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    // Maybe this will come in handy someday.
    return return_value;
}

Plugin::Plugin() {
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif
    debug_print("Plugin loaded");

    redirect_code(0x5B61B0, patch_CStreaming__LoadCdDirectory_5B61B0);
    redirect_code(0x5B61E1, patch_CStreaming__LoadCdDirectory_5B61E1, true);

    redirect_code(0x40E2C5, patch_CStreaming__ConvertBufferToObject_40E2C5, true);

#ifdef _DEBUG
    patch_set_nop(0x406565, 1);
    patch_byte(0x406565 + 1, 0xBD);
    patch_pointer(0x406565 + 2, reinterpret_cast<uintptr_t>(patch_StreamReaderThread_406567));
#endif
}

Plugin::~Plugin() {
    debug_print("Plugin unloaded");
}
