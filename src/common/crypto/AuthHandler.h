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
    AuthHandler() = delete;
    explicit AuthHandler(const std::vector<uint8_t>& key);

    std::vector<uint8_t> GetICV(const std::vector<uint8_t>& data);
    bool VerifyICV(const std::vector<uint8_t>& data);

private:
    std::vector<uint8_t> m_key {};
};

#endif //GCEMU_AUTHHANDLER_H
