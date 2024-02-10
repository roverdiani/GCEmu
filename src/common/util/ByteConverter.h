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

#ifndef GCEMU_BYTECONVERTER_H
#define GCEMU_BYTECONVERTER_H

#include <algorithm>

template<size_t T>
inline void Convert(char* val)
{
    std::swap(*val, *(val + T - 1));
    Convert<T - 2>(val + 1);
}

template<> inline void Convert<0>(char*) {}
template<> inline void Convert<1>(char*) {}

template<typename T>
inline void Apply(T* val)
{
    Convert<sizeof(T)>((char*)(val));
}

template<typename T>
inline void EndianConvertReverse(T& val)
{
    Apply<T>(&val);
}

#endif //GCEMU_BYTECONVERTER_H
