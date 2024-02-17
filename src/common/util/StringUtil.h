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

#ifndef GCEMU_STRINGUTIL_H
#define GCEMU_STRINGUTIL_H

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
#include <utf8.h>

class StringUtil
{
public:
    static std::u16string Utf8To16(const std::string& input)
    {
        return utf8::utf8to16(input);
    }

    static std::string Utf16To8(const std::u16string& input)
    {
        return utf8::utf16to8(input);
    }

    static std::vector<uint8_t> Utf16ToVector(const std::u16string& input)
    {
        // Each char in the u16string is 2 bytes
        std::vector<uint8_t> vector(input.size() * 2);
        memcpy(&(*vector.begin()), input.c_str(), vector.size());

        return vector;
    }

    static std::u16string VectorToUtf16(const std::vector<uint8_t>& input)
    {
        // Each char in the u16string is 2 bytes
        std::u16string str(input.size() / 2, '\0');
        memcpy(&(*str.begin()), &(*input.begin()), input.size());

        return str;
    }

    static std::vector<std::string> StringSplit(const std::string& source, const std::string& separator)
    {
        std::vector<std::string> r;
        std::string s;
        for (char i : source)
        {
            if (separator.find(i) != std::string::npos)
            {
                if (!s.empty())
                    r.push_back(s);

                s.clear();
            }
            else
                s += i;
        }

        if (!s.empty())
            r.push_back(s);

        return r;
    }
};

#endif //GCEMU_STRINGUTIL_H
