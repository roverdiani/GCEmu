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

#include "PacketBuffer.h"
#include <cassert>

PacketBuffer::PacketBuffer(int32_t initialSize) : m_buffer(initialSize, 0)
{
}

uint8_t PacketBuffer::Peak() const
{
    return m_buffer[m_readPosition];
}

size_t PacketBuffer::ReadLengthRemaining() const
{
    return m_writePosition - m_readPosition;
}

void PacketBuffer::Read(char *buffer, int32_t length)
{
    assert(ReadLengthRemaining() >= length);

    if (buffer)
        memcpy(buffer, &m_buffer[m_readPosition], length);

    m_readPosition += length;
}

void PacketBuffer::Write(const char *buffer, int32_t length)
{
    assert(buffer != nullptr && length != 0);

    const size_t newLength = m_writePosition + length;

    if (m_buffer.size() < newLength)
        m_buffer.resize(newLength);

    memcpy(&m_buffer[m_writePosition], buffer, length);
    m_writePosition += length;
}
