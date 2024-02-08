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

#ifndef GCEMU_AUTHHANDLER_H
#define GCEMU_AUTHHANDLER_H

#include <cstdint>
#include <vector>

class AuthHandler
{
public:
    AuthHandler() = default;
    explicit AuthHandler(const std::vector<uint8_t>& hmacKey);

    std::vector<uint8_t> GetHmac(const std::vector<uint8_t>& authData);
    bool VerifyHmac(const std::vector<uint8_t>& packetData);

private:
    // Default HMAC key
    std::vector<uint8_t> m_hmacKey = { 0xC0, 0xD3, 0xBD, 0xC3, 0xB7, 0xCE, 0xB8, 0xB8 };
};

#endif //GCEMU_AUTHHANDLER_H
