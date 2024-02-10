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

#ifndef GCEMU_BYTEBUFFER_H
#define GCEMU_BYTEBUFFER_H

#include <cassert>
#include <vector>
#include "ByteConverter.h"

class ByteBuffer
{
public:
    ByteBuffer& operator <<(uint8_t value)
    {
        Append<uint8_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(uint16_t value)
    {
        Append<uint16_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(uint32_t value)
    {
        Append<uint32_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(uint64_t value)
    {
        Append<uint64_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(int8_t value)
    {
        Append<int8_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(int16_t value)
    {
        Append<int16_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(int32_t value)
    {
        Append<int32_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(int64_t value)
    {
        Append<int64_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(float_t value)
    {
        Append<float_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(double_t value)
    {
        Append<double_t>(value);
        return *this;
    }

    ByteBuffer& operator <<(const std::vector<uint8_t>& data)
    {
        Append(data);
        return *this;
    }

protected:
    explicit ByteBuffer(size_t reservedSize = DEFAULT_SIZE)
    {
        m_storage.reserve(reservedSize);
    }

    const uint8_t* Data()
    {
        return &m_storage[0];
    }

    void Resize(size_t newSize)
    {
        m_storage.resize(newSize);
        m_writePosition = Size();
    }

    void Reserve(size_t size)
    {
        if (size > Size())
            m_storage.reserve(size);
    }

    bool IsEmpty()
    {
        return m_storage.empty();
    }

    size_t GetWritePosition() const
    {
        return m_writePosition;
    }

    size_t Size()
    {
        return m_storage.size();
    }

    void Append(const uint8_t* src, size_t length)
    {
        if (!length)
            return;

        // FIXME: magic number?
        assert((Size() + length) < 10000000);

        if (m_storage.size() < m_writePosition + length)
            m_storage.resize(m_writePosition + length);

        memcpy(&m_storage[m_writePosition], src, length);
        m_writePosition += length;
    }

    void Append(const std::vector<uint8_t>& buffer)
    {
        Append(buffer.data(), buffer.size());
    }

    void Append(ByteBuffer& buffer)
    {
        // FIXME: do we need this check?
        if (buffer.GetWritePosition())
            Append(buffer.Data(), buffer.GetWritePosition());
    }

    static constexpr size_t DEFAULT_SIZE = 0x1000; // 4 kb

    std::vector<uint8_t> m_storage;

private:
    template <typename T>
    void Append(T value)
    {
        EndianConvertReverse(value);
        Append((uint8_t*) &value, sizeof(value));
    }

    size_t m_writePosition = 0;
};

#endif //GCEMU_BYTEBUFFER_H
