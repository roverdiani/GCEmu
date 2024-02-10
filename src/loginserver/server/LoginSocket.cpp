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
#include "../../common/crypto/Generator.h"

LoginSocket::LoginSocket(boost::asio::io_context &ioContext, const std::function<void(Socket *)>& closeHandler) : Socket(ioContext, closeHandler)
{
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

    uint16_t packetLength = (packetData[1] << 8) | packetData[0];
    packetData.resize(packetLength);

    if (!Read((char*)packetData.data() + 2, packetLength - 2))
        return false;

    Packet pkt(m_authHandler, m_cryptoHandler);
    if (!pkt.LoadData(packetData))
        return false;

    return true;
}

void LoginSocket::SendPacket(Packet packet)
{
    if (IsClosed())
        return;

    std::lock_guard<std::mutex> lock(m_loginSocketMutex);
    std::vector<uint8_t> packetData = packet.GetDataToSend(m_prefix, ++m_packetCount);
    Write(reinterpret_cast<const char*>(packetData.data()), (int32_t) packetData.size());
}

void LoginSocket::EventAcceptConnectionNot()
{
    uint16_t newPrefix = Generator::GeneratePrefix();
    std::vector<uint8_t> newAuthKey = Generator::GenerateKey();
    std::vector<uint8_t> newCryptoKey = Generator::GenerateKey();

    Packet pkt(LoginOpcodes::EVENT_ACCEPT_CONNECTION_NOT, false, m_authHandler, m_cryptoHandler);
    pkt << newPrefix;
    pkt << newAuthKey.size();
    pkt << newAuthKey;
    pkt << newCryptoKey.size();
    pkt << newCryptoKey;
    pkt << 0x01; // Unk1
    pkt << 0x00; // Unk2
    pkt << 0x00; // Unk3

    SendPacket(pkt);

    m_authHandler->UpdateHmacKey(newAuthKey);
    m_cryptoHandler->UpdateCryptoKey(newCryptoKey);
    m_prefix = newPrefix;
}
