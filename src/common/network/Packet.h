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
        uint16_t Prefix;
        uint32_t PacketCount;
        uint8_t IV[8];
    };

    struct PacketAuthentication
    {
        uint8_t HMAC[10];
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

    explicit Packet(std::shared_ptr<AuthHandler> authHandler, std::shared_ptr<CryptoHandler> cryptoHandler) : m_authHandler(std::move(authHandler)), m_cryptoHandler(std::move(cryptoHandler))
    {
    }

    explicit Packet(uint8_t opcode, bool isCompressed, std::shared_ptr<AuthHandler> authHandler, std::shared_ptr<CryptoHandler> cryptoHandler, size_t reservedSize = 200) : m_opcode(opcode), m_isCompressed(isCompressed), m_authHandler(std::move(authHandler)), m_cryptoHandler(std::move(cryptoHandler)), ByteBuffer(reservedSize)
    {
    }

    bool LoadData(const std::vector<uint8_t>& data);

    std::vector<uint8_t> GetDataToSend(uint16_t prefix, uint32_t packetCount);

private:
    bool VerifyHandlers();
    void ReadPayload(const std::vector<uint8_t>& packetData);

    PacketHeader m_packetHeader {};

    // Packet Payload
    uint16_t m_opcode = 0;
    uint32_t m_payloadLength = 0;
    bool m_isCompressed = false;

    PacketAuthentication m_packetAuthentication {};

    std::shared_ptr<AuthHandler> m_authHandler = nullptr;
    std::shared_ptr<CryptoHandler> m_cryptoHandler = nullptr;
};

#endif //GCEMU_PACKET_H
