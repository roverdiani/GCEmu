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
#include <iostream>
#include <openssl/provider.h>
#include <openssl/crypto.h>

CryptoHandler::CryptoHandler(const std::vector<uint8_t> &key) : m_key(key)
{
}

std::vector<uint8_t> CryptoHandler::EncryptPacket(const std::vector<uint8_t>& payload, const std::vector<uint8_t>& iv)
{
    std::vector<uint8_t> paddedData = PadData(payload);
    return DesEncryption::EncryptData(paddedData, iv, m_key);
}

std::vector<uint8_t> CryptoHandler::DecryptPacket(const std::vector<uint8_t>& encryptedData, const uint8_t* iv)
{
    std::vector<uint8_t> decryptedData = DesEncryption::DecryptData(encryptedData, iv, m_key);
    return decryptedData;
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

void CryptoHandler::UpdateCryptoKey(const std::vector<uint8_t> &newKey)
{
    m_key = newKey;
}

bool CryptoHandler::InitOpenSSL()
{
    // Starting with OpenSSL 3.0, several deprecated or insecure algorithms were moved into an
    // internal library module called legacy provider, which is not loaded by default.
    // As GC uses the DES algorithm, which is one of the deprecated algorithms, we need to load
    // the provider before trying to use it.
    OSSL_PROVIDER* legacy = OSSL_PROVIDER_load(nullptr, "legacy");
    if (legacy == nullptr)
    {
        std::cout << "CryptoHandler::InitOpenSSL: Error: Legacy Provider loading failed \n";
        return false;
    }

    OSSL_PROVIDER* digest = OSSL_PROVIDER_load(nullptr, "default");
    if (digest == nullptr)
    {
        std::cout << "CryptoHandler::InitOpenSSL: Error: Default Provider loading failed \n";
        return false;
    }
    OPENSSL_init();

    return true;
}
