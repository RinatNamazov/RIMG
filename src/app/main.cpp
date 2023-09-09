/*****************************************************************************
 *
 *  PROJECT:        RIMG
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           app/main.cpp
 *  DESCRIPTION:    Encrypting IMG files
 *  COPYRIGHT:      (c) 2020 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "main.hpp"

#include <Windows.h>

#include <aes.hpp>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "misc/keys.hpp"
#include "misc/types.hpp"

const uint16_t BLOCK_SIZE = 2048;

int main(int argc, char* argv[]) {
    std::cout << "RIMG v.1.0.0 - Encryption IMG archives for GTA:SA. <https://github.com/RinatNamazov/RIMG>\n"
              << "Developer: Rinat Namazov\n"
              << "Copyright (c) 2020 RINWARES <rinwares.com>\n"
              << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: <input_file_path> [output_file_path]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string input_file_path  = argv[1];
    std::string output_file_path = argc >= 3 ? argv[2] : input_file_path + "_packed";

    std::ifstream input_file(input_file_path, std::ios::binary);
    if (!input_file.is_open()) {
        std::cerr << "Could not open input file." << std::endl;
        return EXIT_FAILURE;
    }

    char sig[4];
    input_file.read(reinterpret_cast<char*>(&sig), sizeof(sig));
    if (sig[0] != 'V' && sig[1] != 'E' && sig[2] != 'R' && sig[3] != '2') {
        std::cerr << "Invalid IMG archive format." << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream output_file(output_file_path, std::ios::binary | std::ios::trunc);
    if (!output_file.is_open()) {
        std::cerr << "Could not open output file." << std::endl;
        return EXIT_FAILURE;
    }

    clock_t time_start = clock();

    const char*   copyright_string = "RINWARES";
    const uint8_t copyright_length = 8;
    output_file.write(copyright_string, copyright_length);

    uint32_t entry_count;
    input_file.read(reinterpret_cast<char*>(&entry_count), sizeof(entry_count));

    CryptImgHeader crypt_header    = {0};
    crypt_header.check             = 1703;
    crypt_header.number_of_entries = 0;
    crypt_header.number_of_entries = entry_count;

    struct AES_ctx hdr_ctx;
    AES_init_ctx_iv(&hdr_ctx, hdr_key, iv);
    AES_CBC_encrypt_buffer(&hdr_ctx, reinterpret_cast<uint8_t*>(&crypt_header),
                           sizeof(crypt_header));
    output_file.write(reinterpret_cast<char*>(&crypt_header), sizeof(crypt_header));

    std::vector<DirEntry> file_entries;
    for (uint32_t i = 0; i < entry_count; ++i) {
        DirEntry entry;
        input_file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
#ifdef _DEBUG
        std::cout << "file_name: " << entry.file_name << ", offset: " << entry.offset
                  << ", size_in_archive: " << entry.size_in_archive
                  << ", streaming_size: " << entry.streaming_size << "\n";
#endif
        file_entries.push_back(entry);
    }

    output_file.seekp(copyright_length + sizeof(CryptImgHeader) + entry_count * sizeof(DirEntry),
                      std::ios::beg);

    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_int_distribution<int> distribution(0, 255);

    for (auto& entry : file_entries) {
        std::cout << "packing: " << entry.file_name << "\n";

        uint32_t file_offset = entry.offset * BLOCK_SIZE;
        uint32_t file_size   = entry.streaming_size * BLOCK_SIZE;
        uint8_t* file_buffer
            = new uint8_t[file_size + copyright_length + sizeof(file_size) + sizeof(FileMetaInfo)];

        if ((file_size % AES_BLOCKLEN) != 0) {
            std::cerr << "File size should be multiples of 16 bytes." << std::endl;
            return EXIT_FAILURE;
            // Todo: Add padding.
        }

        memcpy(file_buffer, copyright_string, copyright_length);
        *reinterpret_cast<uint32_t*>(file_buffer + copyright_length) = file_size;
        const uint8_t metadata_size = copyright_length + sizeof(file_size);

        uint8_t fle_key[32];
        for (size_t i = 0; i < sizeof(fle_key); ++i) {
            fle_key[i] = distribution(gen);
        }

        FileMetaInfo meta_info = {0};
        meta_info.file_size    = file_size;
        meta_info.flags        = FILE_FLAG_ENCRYPTED;
        memcpy(&meta_info.key, fle_key, sizeof(fle_key));
        memcpy(file_buffer + metadata_size, &meta_info, sizeof(meta_info));

        input_file.seekg(file_offset);
        input_file.read(reinterpret_cast<char*>(file_buffer + metadata_size + sizeof(meta_info)),
                        file_size);

        output_file.seekp((BLOCK_SIZE - output_file.tellp() % BLOCK_SIZE) % BLOCK_SIZE,
                          std::ios::cur); // Align.

        output_file.write(reinterpret_cast<char*>(file_buffer), metadata_size);

        struct AES_ctx met_ctx;
        AES_init_ctx_iv(&met_ctx, met_key, iv);
        AES_CBC_encrypt_buffer(&met_ctx, reinterpret_cast<uint8_t*>(file_buffer + metadata_size),
                               sizeof(meta_info));
        output_file.write(reinterpret_cast<char*>(file_buffer + metadata_size), sizeof(meta_info));

        uint8_t*       pFileBuff = file_buffer + metadata_size + sizeof(meta_info);
        struct AES_ctx fle_ctx;
        AES_init_ctx(&fle_ctx, fle_key);
        for (uint32_t i = 0; i < (file_size / AES_BLOCKLEN); i++) {
            AES_ECB_encrypt(&fle_ctx, pFileBuff + (i * AES_BLOCKLEN));
        }
        output_file.write(reinterpret_cast<char*>(pFileBuff), file_size);

        output_file.seekp(BLOCK_SIZE - metadata_size - sizeof(meta_info), std::ios::cur); // Align.

        file_size += BLOCK_SIZE;
        entry.offset = (static_cast<uint32_t>(output_file.tellp()) - file_size) / BLOCK_SIZE;
        entry.streaming_size += 1;

        delete[] file_buffer;
    }

    std::cout << "Number of files: " << entry_count << "\n";

    output_file.seekp(copyright_length + sizeof(CryptImgHeader), std::ios::beg);
    for (auto& entry : file_entries) {
        struct AES_ctx elm_ctx;
        AES_init_ctx_iv(&elm_ctx, elm_key, iv);
        AES_CBC_encrypt_buffer(&elm_ctx, reinterpret_cast<uint8_t*>(&entry), sizeof(entry));
        output_file.write(reinterpret_cast<char*>(&entry), sizeof(entry));
    }

    double elapsed_time = static_cast<double>(clock() - time_start) / CLOCKS_PER_SEC;

    std::cout << "Successfully packaged and saved to \"" << output_file_path << "\".\n";
    std::cout << "Total elapsed time: " << std::fixed << std::setprecision(2) << elapsed_time
              << "s\n";

    return EXIT_SUCCESS;
}
