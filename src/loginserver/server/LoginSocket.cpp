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

#include "LoginSocket.h"
#include "LoginOpcodes.h"
#include "../../common/crypto/Security.h"

LoginSocket::LoginSocket(boost::asio::io_context &ioContext, const std::function<void(Socket *)>& closeHandler) : Socket(ioContext, closeHandler)
{
    m_securityAssociation = Security::GetInstance().GetDefaultSecurityAssociation();
}

bool LoginSocket::Open()
{
    if (!Socket::Open())
        return false;

    EventAcceptConnectionNot();

    return true;
}

bool LoginSocket::ProcessIncomingData()
{
    std::vector<uint8_t> packetData(2);
    if (!Read((char*) packetData.data(), 2))
        return false;

    // FIXME: it's probably a bad idea to trust the client always on the packet length
    uint16_t packetLength = (packetData[1] << 8) | packetData[0];
    packetData.resize(packetLength);

    if (!Read((char*)packetData.data() + 2, packetLength - 2))
        return false;

    Packet pkt;
    if (!pkt.LoadData(packetData, m_securityAssociation))
        return false;

    return true;
}

void LoginSocket::SendPacket(Packet packet)
{
    if (IsClosed())
        return;

    std::lock_guard<std::mutex> lock(m_loginSocketMutex);
    std::vector<uint8_t> packetData = packet.GetDataToSend(m_securityAssociation);
    Write(reinterpret_cast<const char*>(packetData.data()), (int32_t) packetData.size());
}

void LoginSocket::EventAcceptConnectionNot()
{
    uint16_t newSpi;
    auto newSa = Security::GetInstance().CreateNewSecurityAssociation(newSpi);
    if (!newSa)
        return;

    Packet pkt(LoginOpcodes::EVENT_ACCEPT_CONNECTION_NOT, false);
    pkt << newSpi;
    pkt << newSa->GetSecurityAssociationData();

    SendPacket(pkt);

    m_securityAssociation = newSa;
}
