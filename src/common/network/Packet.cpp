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

#include "../util/Compressor.h"
#include "Packet.h"
#include <cstring>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

bool Packet::LoadData(const std::vector<uint8_t>& data, const std::shared_ptr<SecurityAssociation>& sa)
{
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
    ReadPayload(data, sa);

    // Copy packet authentication from the end of the packet data
    memcpy(&m_packetAuthentication, &(*(data.end() - sizeof(m_packetAuthentication))), sizeof(m_packetAuthentication));

    // TODO: verify the ICV authentication

    return true;
}

std::vector<uint8_t> Packet::GetDataToSend(const std::shared_ptr<SecurityAssociation>& sa)
{
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

    if (m_isCompressed)
    {
        // TODO: compress packet
    }

    std::vector<uint8_t> iv;
    uint16_t spi;
    uint32_t sequenceNumber;
    std::vector<uint8_t> encryptedPayload = sa->EncryptData(payload, iv, spi, sequenceNumber);

    memcpy(m_packetHeader.IV, &(*iv.begin()), sizeof(m_packetHeader.IV));
    m_packetHeader.Spi = spi;
    m_packetHeader.SequenceNumber = sequenceNumber;

    m_packetHeader.Size = sizeof(m_packetHeader) + encryptedPayload.size() + sizeof(m_packetAuthentication);

    std::vector<uint8_t> data(sizeof(m_packetHeader));
    memcpy(&(*data.begin()), &m_packetHeader, sizeof(m_packetHeader));

    data.insert(data.end(), encryptedPayload.begin(), encryptedPayload.end());

    std::vector<uint8_t> hmac = sa->GetHmac({data.begin() + 2, data.end() });

    data.resize(data.size() + 10);
    memcpy(&(*(data.end() - 10)), hmac.data(), hmac.size());

    return data;
}

void Packet::ReadPayload(const std::vector<uint8_t>& packetData, const std::shared_ptr<SecurityAssociation>& sa)
{
    std::vector<uint8_t> encryptedPayload(packetData.begin() + sizeof(m_packetHeader), packetData.end() - sizeof(m_packetAuthentication));
    std::vector<uint8_t> decryptedPayload = sa->DecryptData(encryptedPayload, m_packetHeader.IV);

    m_opcode = (decryptedPayload[0] << 8) | decryptedPayload[1];

    memcpy(&m_payloadLength, &(*(decryptedPayload.begin() + sizeof(m_opcode))), sizeof(m_payloadLength));
    m_payloadLength = ntohl(m_payloadLength);

    if (m_payloadLength == 0)
        return;

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

std::vector<uint8_t> Packet::GetPayloadData()
{
    return m_storage;
}
