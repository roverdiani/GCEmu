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

#include "AuthHandler.h"
#include "Md5Hmac.h"

AuthHandler::AuthHandler(const std::vector<uint8_t>& hmacKey) : m_hmacKey(hmacKey)
{
}

std::vector<uint8_t> AuthHandler::GetHmac(const std::vector<uint8_t>& authData)
{
    return Md5Hmac::ComputeHmac(authData, m_hmacKey, 10);
}

bool AuthHandler::VerifyHmac(const std::vector<uint8_t>& packetData)
{
    std::vector<uint8_t> storedHmac(packetData.end() - 10, packetData.end());

    std::vector<uint8_t> authData(packetData.begin() + 2, packetData.end() - 10 - 2);
    std::vector<uint8_t> expectedHmac = Md5Hmac::ComputeHmac(authData, m_hmacKey, 10);

    return storedHmac == expectedHmac;
}

void AuthHandler::UpdateHmacKey(const std::vector<uint8_t> &newKey)
{
    m_hmacKey = newKey;
}
