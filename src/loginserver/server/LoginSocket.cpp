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
#include "AccountVerificationResults.h"
#include "../../common/crypto/Security.h"
#include "../../common/database/Database.h"
#include "../../common/util/StringUtil.h"
#include <spdlog/spdlog.h>

extern Database database;

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

    switch (pkt.GetOpcode())
    {
        case EVENT_HEART_BIT_NOT:
            HandleEventHeartBitNot();
            break;
        case EVENT_ACCEPT_CONNECTION_NOT:
            break;
        case ENU_VERIFY_ACCOUNT_REQ:
            HandleEnuVerifyAccountReq(pkt);
        default:
            return true;
    }

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

void LoginSocket::HandleEventHeartBitNot()
{
    spdlog::info("EVENT_HEART_BIT_NOT");
}

void LoginSocket::HandleEnuVerifyAccountReq(Packet &pkt)
{
    spdlog::info("ENU_VERIFY_ACCOUNT_REQ");
    // The minimum packet length for this is 61 bytes
    if (pkt.GetPayloadLength() < 61)
    {
        spdlog::error("LoginSocket::HandleEnuVerifyAccountReq invalid size.");
        return;
    }

    uint32_t usernameLength;
    pkt >> usernameLength;
    std::string username = pkt.ReadString(usernameLength);

    uint32_t passwordHashLength;
    pkt >> passwordHashLength;
    std::vector<uint8_t> passwordHash = pkt.ReadVector(passwordHashLength);

    Packet outPacket((uint16_t) ENU_VERIFY_ACCOUNT_ACK, false);
    auto queryResult = database.PreparedQuery("SELECT * FROM account WHERE username = '%s'", username.c_str());
    if (!queryResult)
    {
        // No account found on the database with the provided data.
        spdlog::info("LoginSocket::HandleEnuVerifyAccountReq: username not found.");
        outPacket << AccountVerificationResults::ERR_USER_NOT_FOUND;
        outPacket.WriteU16String(StringUtil::Utf8To16(username));
        outPacket << (uint32_t) 0x00; // NMPasswd String Length - unused here
        outPacket << (uint8_t) false; // IsMale - the client sends this as the default value
        outPacket << 0x14; // Age - the client sends this as the default value
        SendPacket(outPacket);
        return;
    }

    spdlog::info("LoginSocket::HandleEnuVerifyAccountReq: username found.");
}
