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

#ifndef GCEMU_DESENCRYPTION_H
#define GCEMU_DESENCRYPTION_H

#include <cstdint>
#include <vector>

class DesEncryption
{
public:
    static std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& key);
    static std::vector<uint8_t> DecryptData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& key);
};

#endif //GCEMU_DESENCRYPTION_H
