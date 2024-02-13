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

#include "CryptoHandler.h"
#include "DesEncryption.h"

CryptoHandler::CryptoHandler(const std::vector<uint8_t> &key) : m_key(key)
{
}

std::vector<uint8_t> CryptoHandler::EncryptData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& iv)
{
    std::vector<uint8_t> paddedData = PadData(data);
    return DesEncryption::EncryptData(paddedData, iv, m_key);
}

std::vector<uint8_t> CryptoHandler::DecryptData(const std::vector<uint8_t>& data, const uint8_t* iv)
{
    return DesEncryption::DecryptData(data, iv, m_key);
}

std::vector<uint8_t> CryptoHandler::PadData(std::vector<uint8_t> data)
{
    // Get the distance from the size to the next number divisible by the block size (8).
    size_t distance = 8 - (data.size() % 8);
    size_t paddingLength = distance >= 3 ? distance : 8 + distance;

    std::vector<uint8_t> padding(paddingLength);
    for (size_t i = 0; i < paddingLength - 1; i++)
        padding[i] = i;

    // The last byte should be equal to the one before it.
    padding[paddingLength - 1] = padding[paddingLength - 2];

    // Concat the data with the padding
    data.insert(data.end(), padding.begin(), padding.end());

    return data;
}
