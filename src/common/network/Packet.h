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

#ifndef GCEMU_PACKET_H
#define GCEMU_PACKET_H

#include "../util/ByteBuffer.h"
#include "../crypto/AuthHandler.h"
#include "../crypto/CryptoHandler.h"
#include "../crypto/SecurityAssociation.h"
#include <cstdint>
#include <memory>

class Packet : public ByteBuffer
{
private:
#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif
    struct PacketHeader
    {
        uint16_t Size;
        uint16_t Spi;
        uint32_t SequenceNumber;
        uint8_t IV[8];
    };

    struct PacketAuthentication
    {
        uint8_t ICV[10];
    };
#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

public:
    // Empty packet container
    Packet() : ByteBuffer(0)
    {
    }

    explicit Packet(uint8_t opcode, bool isCompressed, size_t reservedSize = 200) : m_opcode(opcode), m_isCompressed(isCompressed), ByteBuffer(reservedSize)
    {
    }

    bool LoadData(const std::vector<uint8_t>& data, const std::shared_ptr<SecurityAssociation>& sa);

    std::vector<uint8_t> GetDataToSend(const std::shared_ptr<SecurityAssociation>& sa);
    std::vector<uint8_t> GetPayloadData();

private:
    void ReadPayload(const std::vector<uint8_t>& packetData, const std::shared_ptr<SecurityAssociation>& sa);

    PacketHeader m_packetHeader {};

    // Packet Payload
    uint16_t m_opcode = 0;
    uint32_t m_payloadLength = 0;
    bool m_isCompressed = false;

    PacketAuthentication m_packetAuthentication {};
};

#endif //GCEMU_PACKET_H
