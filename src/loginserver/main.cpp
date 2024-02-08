// This file is part of the GCEmu Project.
// This is the main file for the loginserver project.
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

#include <iostream>
#include "../common/config/ConfigHandler.h"
#include "../common/network/TcpListener.h"
#include "LoginSocket.h"
#include "../common/crypto/CryptoHandler.h"

int main(int argc, char* argv[])
{
    std::cout << R"(   ______ ______ ______                 )" << std::endl;
    std::cout << R"(  / ____// ____// ____/____ ___   __  __)" << std::endl;
    std::cout << R"( / / __ / /    / __/  / __ `__ \ / / / /)" << std::endl;
    std::cout << R"(/ /_/ // /___ / /___ / / / / / // /_/ / )" << std::endl;
    std::cout << R"(\____/ \____//_____//_/ /_/ /_/ \__,_/  )" << std::endl;
    std::cout << std::endl;

    std::cout << "GCEmu Login Server" << std::endl;
    std::cout << "<Ctrl-C> to stop." << std::endl;

    std::cout << std::endl;

    // Set and parse the configuration file.
    if (SConfigHandler.SetConfigFile("loginserver.conf.json"))
        std::cout << "Config file loaded." << std::endl;
    else
    {
        std::cout << "Check if the config file is correct." << std::endl;
        return -1;
    }

    std::cout << "Initializing OpenSSL... ";
    if (!CryptoHandler::InitOpenSSL())
    {
        std::cout << "Failed to init OpenSSL. Exiting!" << std::endl;
        return -1;
    }
    std::cout << "done!" << std::endl;

    TcpListener<LoginSocket> listener("",
                                      SConfigHandler.GetInt("port", 9501),
                                      SConfigHandler.GetInt("network_threads", 1));

    while (true);

    return 0;
}
