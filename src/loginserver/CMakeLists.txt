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
find_package(spdlog REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${nlohmann_json_INCLUDE_DIRS} ${OpenSSL_INCLUDE_DIRS} ${spdlog_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

add_executable(loginserver main.cpp ../common/config/ConfigHandler.cpp ../common/config/ConfigHandler.h ../common/network/TcpListener.h ../common/network/NetworkThread.h ../common/network/Socket.cpp ../common/network/Socket.h ../common/network/PacketBuffer.cpp ../common/network/PacketBuffer.h server/LoginSocket.cpp server/LoginSocket.h ../common/crypto/AuthHandler.cpp ../common/crypto/AuthHandler.h ../common/crypto/Md5Hmac.h ../common/crypto/CryptoHandler.cpp ../common/crypto/CryptoHandler.h ../common/crypto/DesEncryption.cpp ../common/crypto/DesEncryption.h ../common/util/ByteBuffer.h ../common/network/Packet.h ../common/crypto/Generator.h server/LoginOpcodes.h server/LoginOpcodes.cpp server/OpcodeMap.h server/OpcodeMap.cpp server/LoginSession.cpp server/LoginSession.h ../common/util/ByteConverter.h ../common/network/Packet.cpp ../common/util/Compressor.cpp ../common/util/Compressor.h)
target_link_libraries(loginserver openssl::openssl spdlog::spdlog ZLIB::ZLIB)