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

#ifndef GCEMU_LOGINSOCKET_H
#define GCEMU_LOGINSOCKET_H

#include "../../common/crypto/AuthHandler.h"
#include "../../common/crypto/CryptoHandler.h"
#include "../../common/network/Socket.h"
#include "../../common/network/Packet.h"
#include <mutex>
#include <boost/asio.hpp>

class LoginSocket : public Socket
{
public:
    LoginSocket(boost::asio::io_context& ioContext, const std::function<void(Socket*)>& closeHandler);

    bool Open() override;

    void SendPacket(Packet packet);

private:
    bool ProcessIncomingData() override;

    void EventAcceptConnectionNot();

    std::shared_ptr<AuthHandler> m_authHandler = std::make_shared<AuthHandler>();
    std::shared_ptr<CryptoHandler> m_cryptoHandler = std::make_shared<CryptoHandler>();

    uint16_t m_prefix = 0x00;
    uint32_t m_packetCount = 0x00;

    std::mutex m_loginSocketMutex;
};

#endif //GCEMU_LOGINSOCKET_H
