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

#include "Security.h"
#include <openssl/provider.h>
#include <openssl/crypto.h>
#include <spdlog/spdlog.h>

Security::Security()
{
    uint16_t spi;
    std::shared_ptr<SecurityAssociation> sa = std::make_shared<SecurityAssociation>(spi, true);
    m_securityAssociationMap.insert(std::pair<uint16_t, std::shared_ptr<SecurityAssociation>>(spi, sa));
}

std::shared_ptr<SecurityAssociation> Security::CreateNewSecurityAssociation(uint16_t& newSpi)
{
    std::lock_guard<std::mutex> lock(m_securityMutex);

    std::shared_ptr<SecurityAssociation> sa = std::make_shared<SecurityAssociation>(newSpi);

    if (m_securityAssociationMap.find(newSpi) != m_securityAssociationMap.end())
    {
        spdlog::error("Security Association with specified SPI (0x{0}) already exists!", newSpi);
        return nullptr;
    }

    m_securityAssociationMap.insert(std::pair<uint16_t, std::shared_ptr<SecurityAssociation>>(newSpi, sa));
    return sa;
}

std::shared_ptr<SecurityAssociation> Security::GetSecurityAssociation(uint16_t spi)
{
    std::lock_guard<std::mutex> lock(m_securityMutex);

    auto itr = m_securityAssociationMap.find(spi);
    if (itr == m_securityAssociationMap.end())
    {
        spdlog::error("Could not find the specified Security Association. SPI 0x{0}", spi);
        return nullptr;
    }

    return itr->second;
}

std::shared_ptr<SecurityAssociation> Security::GetDefaultSecurityAssociation()
{
    return GetSecurityAssociation(0x0000);
}

bool Security::InitOpenSSL()
{
    // Starting with OpenSSL 3.0, several deprecated or insecure algorithms were moved into an
    // internal library module called legacy provider, which is not loaded by default.
    // As GC uses the DES algorithm, which is one of the deprecated algorithms, we need to load
    // the provider before trying to use it.
    OSSL_PROVIDER* legacy = OSSL_PROVIDER_load(nullptr, "legacy");
    if (legacy == nullptr)
    {
        spdlog::error("CryptoHandler::InitOpenSSL: Error: Legacy Provider loading failed");
        return false;
    }

    OSSL_PROVIDER* digest = OSSL_PROVIDER_load(nullptr, "default");
    if (digest == nullptr)
    {
        spdlog::error("CryptoHandler::InitOpenSSL: Error: Default Provider loading failed");
        return false;
    }
    OPENSSL_init();

    return true;
}
