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
#include "../common/crypto/CryptoHandler.h"

LoginSocket::LoginSocket(boost::asio::io_context &ioContext, const std::function<void(Socket *)>& closeHandler) : Socket(ioContext, closeHandler)
{
}

bool LoginSocket::Open()
{
    if (!Socket::Open())
        return false;

    return true;
}

bool LoginSocket::ProcessIncomingData()
{
    while (ReadLengthRemaining() > 0)
    {
        char buf[1];
        Read(buf, 1);
        printf("0x%02X ", buf[0]);
    }

    return true;
}
