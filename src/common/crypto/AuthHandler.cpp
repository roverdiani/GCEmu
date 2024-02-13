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

AuthHandler::AuthHandler(const std::vector<uint8_t>& key) : m_key(key)
{
}

std::vector<uint8_t> AuthHandler::GetICV(const std::vector<uint8_t>& data)
{
    return Md5Hmac::ComputeHmac(data, m_key, 10);
}

bool AuthHandler::VerifyICV(const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> storedICV(data.end() - 10, data.end());

    std::vector<uint8_t> authData(data.begin() + 2, data.end() - 10 - 2);
    std::vector<uint8_t> expectedICV = Md5Hmac::ComputeHmac(authData, m_key, 10);

    return storedICV == expectedICV;
}
