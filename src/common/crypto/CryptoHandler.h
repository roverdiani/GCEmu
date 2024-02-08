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

#ifndef GCEMU_CRYPTOHANDLER_H
#define GCEMU_CRYPTOHANDLER_H

#include <cstdint>
#include <vector>

class CryptoHandler
{
public:
    CryptoHandler() = default;
    explicit CryptoHandler(const std::vector<uint8_t>& key);

    static bool InitOpenSSL();

    std::vector<uint8_t> EncryptPacket(const std::vector<uint8_t>& payload, const std::vector<uint8_t>& iv);
    std::vector<uint8_t> DecryptPacket(const std::vector<uint8_t>& packetData);

    static std::vector<uint8_t> PadData(std::vector<uint8_t> data);

private:
    // Default HMAC key
    std::vector<uint8_t> m_key = { 0xC7, 0xD8, 0xC4, 0xBF, 0xB5, 0xE9, 0xC0, 0xFD };
};

#endif //GCEMU_CRYPTOHANDLER_H
