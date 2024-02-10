// This file is part of the GCEmu Project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef GCEMU_COMPRESSOR_H
#define GCEMU_COMPRESSOR_H

#include <cstdint>
#include <vector>
#include <zlib.h>
#include <spdlog/spdlog.h>

class Compressor
{
public:
    static std::vector<uint8_t> CompressData(const std::vector<uint8_t>& data)
    {
        // According to https://bobobobo.wordpress.com/2008/02/23/how-to-use-zlib/,
        // the array that will hold the compressed data must be AT LEAST 0.1% larger
        // than the original size of the data, plus 12 extra bytes.
        uint32_t compressedDataSize = (uint32_t) ((uint32_t) data.size() * 1.1) + 12;

        std::vector<uint8_t> compressedData(compressedDataSize);
        int32_t result = compress2(&compressedData[0], reinterpret_cast<uLongf *>(&compressedDataSize),
                                   data.data(), data.size(), Z_NO_COMPRESSION);

        switch (result)
        {
            case Z_MEM_ERROR:
                spdlog::error("Compressor::CompressData: out of memory!");
                return std::vector<uint8_t> {};
                break;

            case Z_BUF_ERROR:
                spdlog::error("Compressor::CompressData: insufficient buffer length.");
                return std::vector<uint8_t> {};
                break;

            case Z_OK:
            default:
                break;
        }

        // After the compression, ZLIB will update the compressedDataSize variable with
        // the actual size of the resulting data.
        compressedData.resize(compressedDataSize);

        return compressedData;
    }

    static std::vector<uint8_t> DecompressData(const std::vector<uint8_t>& data, uint32_t decompressedSize)
    {
        uint32_t decompressedResultingSize;
        std::vector<uint8_t> decompressedData(decompressedSize);
        int32_t result = uncompress(&decompressedData[0], reinterpret_cast<uLongf *>(&decompressedResultingSize),
                                   data.data(), data.size());

        switch (result)
        {
            case Z_MEM_ERROR:
                spdlog::error("Compressor::DecompressData: out of memory!");
                return std::vector<uint8_t> {};
                break;

            case Z_BUF_ERROR:
                spdlog::error("Compressor::DecompressData: insufficient buffer length.");
                return std::vector<uint8_t> {};
                break;

            case Z_OK:
            default:
                break;
        }

        // After the decompression, ZLIB store the resulting data size in the decompressedResultingSize
        // variable. This should be equal to decompressedSize, which is given to us by the client.
        if (decompressedResultingSize != decompressedSize)
        {
            spdlog::error("Compressor::DecompressData: resulting data size mismatch.");
            return std::vector<uint8_t> {};
        }

        return decompressedData;
    }
};

#endif //GCEMU_COMPRESSOR_H
