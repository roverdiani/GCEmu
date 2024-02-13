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

#ifndef GCEMU_SECURITY_H
#define GCEMU_SECURITY_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include "SecurityAssociation.h"

class Security
{
public:
    Security(Security const&) = delete;
    void operator =(Security const&) = delete;

    static Security& GetInstance()
    {
        static Security instance;
        return instance;
    }

    static bool InitOpenSSL();

    std::shared_ptr<SecurityAssociation> CreateNewSecurityAssociation(uint16_t& newSpi);
    std::shared_ptr<SecurityAssociation> GetSecurityAssociation(uint16_t spi);
    std::shared_ptr<SecurityAssociation> GetDefaultSecurityAssociation();

private:
    Security();

    std::map<uint16_t, std::shared_ptr<SecurityAssociation>> m_securityAssociationMap;

    std::mutex m_securityMutex;
};

#endif //GCEMU_SECURITY_H
