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

#ifndef GCEMU_SECURITYASSOCIATION_H
#define GCEMU_SECURITYASSOCIATION_H

#include <cstdint>
#include <mutex>
#include "AuthHandler.h"
#include "CryptoHandler.h"
#include "Generator.h"
#include "../util/ByteBuffer.h"

#define REPLAY_WINDOW_SIZE  32

class SecurityAssociation
{
public:
    SecurityAssociation() = delete;

    explicit SecurityAssociation(uint16_t& spi, bool defaultKeys = false);

    std::vector<uint8_t> GetSecurityAssociationData();

    std::vector<uint8_t> EncryptData(const std::vector<uint8_t>& data, std::vector<uint8_t>& iv, uint16_t& spi, uint32_t& sequenceNumber);
    std::vector<uint8_t> DecryptData(const std::vector<uint8_t>& data, const uint8_t* iv);
    std::vector<uint8_t> GetHmac(const std::vector<uint8_t>& data);
    bool IsValidSequenceNumber(uint32_t sequenceNumber) const;

private:
    uint16_t m_spi = 0;
    std::vector<uint8_t> m_authenticationKey {};
    std::vector<uint8_t> m_encryptionKey {};
    uint32_t m_sequenceNumber = 0;
    uint32_t m_lastSequenceNumber = 0;
    uint32_t m_replayWindowMask = 0;

    std::shared_ptr<AuthHandler> m_authHandler = nullptr;
    std::shared_ptr<CryptoHandler> m_cryptoHandler = nullptr;

    std::mutex m_securityAssociationMutex;
};

#endif //GCEMU_SECURITYASSOCIATION_H
