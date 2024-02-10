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

#include "DesEncryption.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <spdlog/spdlog.h>

std::vector<uint8_t>
DesEncryption::EncryptData(const std::vector<uint8_t> &data, const std::vector<uint8_t> &iv, const std::vector<uint8_t> &key)
{
    size_t bufLength = data.size();
    uint8_t *buf = (uint8_t *) malloc(bufLength);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    if (EVP_EncryptInit_ex(ctx, EVP_des_cbc(), nullptr, key.data(), iv.data()) != 1)
    {
        spdlog::error("DesEncryption::EncryptData: Error: DES init error!");
        return std::vector<uint8_t>();
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int32_t totalLength = 0;
    int32_t len = 0;
    if (EVP_EncryptUpdate(ctx, buf, &len, data.data(), (int) data.size()) != 1)
    {
        spdlog::error("DesEncryption::EncryptData: Error: Encrypt error!");
        return std::vector<uint8_t>();
    }
    totalLength += len;

    if (EVP_EncryptFinal_ex(ctx, buf + len, &len) != 1)
    {
        spdlog::error("DesEncryption::EncryptData: Error: Encrypt final error!");
        return std::vector<uint8_t>();
    }
    totalLength += len;

    EVP_CIPHER_CTX_free(ctx);

    std::vector<uint8_t> encryptedData(totalLength);
    for (int32_t i = 0; i < totalLength; i++)
        encryptedData[i] = buf[i];

    return encryptedData;
}

std::vector<uint8_t>
DesEncryption::DecryptData(const std::vector<uint8_t> &data, const uint8_t* iv, const std::vector<uint8_t> &key)
{
    size_t bufLength = data.size() + 64 - 1;
    uint8_t *buf = (uint8_t *) malloc(bufLength);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    if (EVP_DecryptInit_ex(ctx, EVP_des_cbc(), nullptr, key.data(), iv) != 1)
    {
        spdlog::error("DesEncryption::DecryptData: Error: DES init error!");
        return std::vector<uint8_t>();
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int32_t totalLength = 0;
    int32_t len = 0;
    if (EVP_DecryptUpdate(ctx, buf, &len, data.data(), (int) data.size()) != 1)
    {
        spdlog::error("DesEncryption::DecryptData: Error: Decrypt error!");
        return std::vector<uint8_t>();
    }
    totalLength += len;

    if (EVP_DecryptFinal_ex(ctx, buf + len, &len) != 1)
    {
        spdlog::error("DesEncryption::DecryptData: Error: Decrypt final error!");
        return std::vector<uint8_t>();
    }
    totalLength += len;

    EVP_CIPHER_CTX_free(ctx);

    std::vector<uint8_t> decryptedData(totalLength);
    for (int32_t i = 0; i < totalLength; i++)
        decryptedData[i] = buf[i];

    return decryptedData;
}
