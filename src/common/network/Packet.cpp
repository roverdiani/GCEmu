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

#include "../crypto/Generator.h"
#include "../util/Compressor.h"
#include "Packet.h"
#include <cstring>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

bool Packet::LoadData(const std::vector<uint8_t> &data)
{
    if (!VerifyHandlers())
        return false;

    // The minimum packet size is the sum of the size of PacketHeader, opcode (2 bytes), payload length (4 bytes)
    // and the size of PacketAuthentication.
    size_t minPacketSize = sizeof(PacketHeader) + 6 + sizeof(PacketAuthentication);
    if (data.size() < minPacketSize)
    {
        spdlog::error("Packet::LoadData: Error: packet doesn't have enough data.");
        return false;
    }

    // Copy packet header from the start of the packet data
    memcpy(&m_packetHeader, &(*data.begin()), sizeof(m_packetHeader));

    // Read packet payload
    ReadPayload(data);

    // Copy packet authentication from the end of the packet data
    memcpy(&m_packetAuthentication, &(*(data.end() - sizeof(m_packetAuthentication))), sizeof(m_packetAuthentication));

    // TODO: verify the HMAC authentication

    return true;
}

std::vector<uint8_t> Packet::GetDataToSend(uint16_t prefix, uint32_t packetCount)
{
    auto iv = Generator::GenerateIV();
    memcpy(m_packetHeader.IV, &(*iv.begin()), sizeof(m_packetHeader.IV));
    m_packetHeader.Prefix = prefix;
    m_packetHeader.PacketCount = packetCount;

    m_payloadLength = Size();
    std::vector<uint8_t> payload;
    payload.push_back(m_opcode >> 8);
    payload.push_back(m_opcode);
    payload.push_back(m_payloadLength >> 24);
    payload.push_back(m_payloadLength >> 16);
    payload.push_back(m_payloadLength >> 8);
    payload.push_back(m_payloadLength);
    payload.push_back(m_isCompressed);
    payload.resize(payload.size() + Size());
    memcpy(&(*(payload.begin() + 7)), Data(), Size());
    std::vector<uint8_t> encryptedPayload = m_cryptoHandler->EncryptPacket(payload, iv);

    if (m_isCompressed)
    {
        // TODO: compress packet
    }

    m_packetHeader.Size = sizeof(m_packetHeader) + encryptedPayload.size() + sizeof(m_packetAuthentication);

    std::vector<uint8_t> data(sizeof(m_packetHeader));
    memcpy(&(*data.begin()), &m_packetHeader, sizeof(m_packetHeader));

    data.insert(data.end(), encryptedPayload.begin(), encryptedPayload.end());

    std::vector<uint8_t> hmac = m_authHandler->GetHmac({data.begin() + 2, data.end() });

    data.resize(data.size() + 10);
    memcpy(&(*(data.end() - 10)), hmac.data(), hmac.size());

    return data;
}

bool Packet::VerifyHandlers()
{
    if (!m_authHandler)
    {
        spdlog::error("Packet::LoadData: Error: invalid AuthHandler.");
        return false;
    }

    if (!m_cryptoHandler)
    {
        spdlog::error("Packet::LoadData: Error: invalid CryptoHandler.");
        return false;
    }

    return true;
}

void Packet::ReadPayload(const std::vector<uint8_t> &packetData)
{
    std::vector<uint8_t> encryptedPayload(packetData.begin() + sizeof(m_packetHeader), packetData.end() - sizeof(m_packetAuthentication));
    std::vector<uint8_t> decryptedPayload = m_cryptoHandler->DecryptPacket(encryptedPayload, m_packetHeader.IV);

    m_opcode = (decryptedPayload[0] << 8) | decryptedPayload[1];

    memcpy(&m_payloadLength, &(*(decryptedPayload.begin() + sizeof(m_opcode))), sizeof(m_payloadLength));
    m_payloadLength = ntohl(m_payloadLength);

    m_isCompressed = decryptedPayload[6];
    if (!m_isCompressed)
        Append(std::vector<uint8_t> {decryptedPayload.begin() + sizeof(m_opcode) + sizeof(m_payloadLength) + sizeof(m_isCompressed), decryptedPayload.end()});
    else
    {
        uint32_t decompressedPayloadSize =
                decryptedPayload[10] << 24 |
                decryptedPayload[9] << 16 |
                decryptedPayload[8] << 8 |
                decryptedPayload[7];

        std::vector<uint8_t> compressedPayload {decryptedPayload.begin() + 11, decryptedPayload.end() - 3};
        std::vector<uint8_t> decompressedPayload = Compressor::DecompressData(compressedPayload, decompressedPayloadSize);
        Append(decompressedPayload);
        m_payloadLength = decryptedPayload.size();
    }
}