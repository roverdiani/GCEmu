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

#ifndef GCEMU_MD5HMAC_H
#define GCEMU_MD5HMAC_H

#include <cstdint>
#include <vector>
#include <openssl/hmac.h>
#include <iostream>
#include <boost/algorithm/hex.hpp>

class Md5Hmac
{
public:
    static std::vector<uint8_t> ComputeHmac(const std::vector<uint8_t>& data, const std::vector<uint8_t>& hmacKey, int32_t digestSize)
    {
        assert(digestSize > 0 && digestSize <= 16);

        uint8_t* hmac = HMAC(EVP_md5(), hmacKey.data(), (int) hmacKey.size(), data.data(), data.size(), nullptr, nullptr);
        std::vector<uint8_t> truncatedHmac(digestSize);
        for (int32_t i = 0; i < digestSize; i++)
            truncatedHmac[i] = hmac[i];

        return truncatedHmac;
    }
};

#endif //GCEMU_MD5HMAC_H
