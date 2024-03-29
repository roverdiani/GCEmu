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

#include "../../common/network/Socket.h"
#include "../../common/network/Packet.h"
#include "../../common/crypto/SecurityAssociation.h"
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

    void HandleEventHeartBitNot();
    void HandleEnuVerifyAccountReq(Packet& pkt);

    std::shared_ptr<SecurityAssociation> m_securityAssociation = nullptr;

    std::mutex m_loginSocketMutex;
};

#endif //GCEMU_LOGINSOCKET_H
