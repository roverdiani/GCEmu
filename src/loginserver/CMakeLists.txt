# This file is part of the GCEmu Project.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(loginserver)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${nlohmann_json_INCLUDE_DIRS} ${OpenSSL_INCLUDE_DIRS})

add_executable(loginserver main.cpp ../common/config/ConfigHandler.cpp ../common/config/ConfigHandler.h ../common/network/TcpListener.h ../common/network/NetworkThread.h ../common/network/Socket.cpp ../common/network/Socket.h ../common/network/PacketBuffer.cpp ../common/network/PacketBuffer.h LoginSocket.cpp LoginSocket.h ../common/crypto/AuthHandler.cpp ../common/crypto/AuthHandler.h ../common/crypto/Md5Hmac.h ../common/crypto/CryptoHandler.cpp ../common/crypto/CryptoHandler.h ../common/crypto/DesEncryption.cpp ../common/crypto/DesEncryption.h)
target_link_libraries(loginserver openssl::openssl)