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
    CryptoHandler() = delete;
    explicit CryptoHandler(const std::vector<uint8_t>& key);

    std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& iv);
    std::vector<uint8_t> DecryptData(const std::vector<uint8_t>& data, const uint8_t* iv);

    static std::vector<uint8_t> PadData(std::vector<uint8_t> data);

private:
    std::vector<uint8_t> m_key {};
};

#endif //GCEMU_CRYPTOHANDLER_H
