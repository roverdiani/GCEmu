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

#ifndef GCEMU_PACKETBUFFER_H
#define GCEMU_PACKETBUFFER_H

#include <cstdint>
#include <vector>

#define DEFAULT_BUFFER_SIZE 6144

class PacketBuffer
{
    friend class Socket;

public:
    explicit PacketBuffer(int32_t initialSize = DEFAULT_BUFFER_SIZE);

    uint8_t Peak() const;

    size_t ReadLengthRemaining() const;

    void Read(char* buffer, int32_t length);
    void Write(const char* buffer, int32_t length);

private:
    size_t m_writePosition = 0;
    size_t m_readPosition = 0;

    std::vector<uint8_t> m_buffer;
};

#endif //GCEMU_PACKETBUFFER_H
