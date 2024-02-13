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

#include "SecurityAssociation.h"

SecurityAssociation::SecurityAssociation(uint16_t &spi, bool defaultKeys)
{
    if (defaultKeys)
    {
        m_spi = 0;
        m_authenticationKey = std::vector<uint8_t> { 0xC0, 0xD3, 0xBD, 0xC3, 0xB7, 0xCE, 0xB8, 0xB8 };
        m_encryptionKey = std::vector<uint8_t> { 0xC7, 0xD8, 0xC4, 0xBF, 0xB5, 0xE9, 0xC0, 0xFD };
    }
    else
    {
        m_spi = Generator::GeneratePrefix();
        m_authenticationKey = Generator::GenerateKey();
        m_encryptionKey = Generator::GenerateKey();
    }

    spi = m_spi;
    m_authHandler = std::make_shared<AuthHandler>(m_authenticationKey);
    m_cryptoHandler = std::make_shared<CryptoHandler>(m_encryptionKey);
}

std::vector<uint8_t> SecurityAssociation::GetSecurityAssociationData()
{
    std::lock_guard<std::mutex> lock(m_securityAssociationMutex);

    ByteBuffer buffer;

    buffer << (uint32_t) m_authenticationKey.size();
    buffer << m_authenticationKey;
    buffer << (uint32_t) m_encryptionKey.size();
    buffer << m_encryptionKey;
    buffer << ++m_sequenceNumber;
    buffer << m_lastSequenceNumber;
    buffer << m_replayWindowMask;

    return buffer.GetData();
}

std::vector<uint8_t>
SecurityAssociation::EncryptData(const std::vector<uint8_t> &data, std::vector<uint8_t> &iv, uint16_t &spi, uint32_t &sequenceNumber)
{
    std::lock_guard<std::mutex> lock(m_securityAssociationMutex);

    iv = Generator::GenerateIV();
    spi = m_spi;
    sequenceNumber = ++m_sequenceNumber;
    return m_cryptoHandler->EncryptData(data, iv);
}

std::vector<uint8_t> SecurityAssociation::DecryptData(const std::vector<uint8_t> &data, const uint8_t *iv)
{
    return m_cryptoHandler->DecryptData(data, iv);
}

std::vector<uint8_t> SecurityAssociation::GetHmac(const std::vector<uint8_t> &data)
{
    std::lock_guard<std::mutex> lock(m_securityAssociationMutex);
    return m_authHandler->GetICV(data);
}

bool SecurityAssociation::IsValidSequenceNumber(uint32_t sequenceNumber) const
{
    if (sequenceNumber == 0)
        return false;

    if (sequenceNumber > m_lastSequenceNumber)
        return true;

    uint32_t gap = m_lastSequenceNumber - sequenceNumber;
    if (gap >= REPLAY_WINDOW_SIZE)
        return false;

    // Checks if the bit corresponding to the packet hasn't been marked
    return ((1 << gap) & m_replayWindowMask) == 0;
}
