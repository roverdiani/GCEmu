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

#ifndef GCEMU_NETWORKTHREAD_H
#define GCEMU_NETWORKTHREAD_H

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <boost/asio.hpp>
#include "Socket.h"

template <typename SocketType>
class NetworkThread
{
public:
    NetworkThread();
    ~NetworkThread();

    size_t Size() const;

    std::shared_ptr<SocketType> CreateSocket();
    void RemoveSocket(Socket *socket);

private:
    boost::asio::io_context m_ioContext;
    std::shared_ptr<boost::asio::io_context::work> m_work;
    std::thread m_serviceThread;
    std::unordered_set<std::shared_ptr<SocketType>> m_sockets;

    std::mutex m_socketLock;
};

template <typename SocketType>
NetworkThread<SocketType>::NetworkThread() : m_work(std::make_unique<boost::asio::io_context::work>(m_ioContext)),
                                 m_serviceThread([this] { boost::system::error_code ec; this->m_ioContext.run(); })
{
}

template <typename SocketType>
size_t NetworkThread<SocketType>::Size() const
{
    return m_sockets.size();
}

template <typename SocketType>
NetworkThread<SocketType>::~NetworkThread()
{
    for (auto & socket : m_sockets)
    {
        if (!socket->IsClosed())
            socket->Close();
    }

    m_ioContext.stop();
    if (m_serviceThread.joinable())
        m_serviceThread.join();
}

template <typename SocketType>
std::shared_ptr<SocketType> NetworkThread<SocketType>::CreateSocket()
{
    std::lock_guard<std::mutex> lock(m_socketLock);

    auto const i = m_sockets.emplace(std::make_shared<SocketType>(m_ioContext, [this] (Socket* socket) { this->RemoveSocket(socket); }));

    return *i.first;
}

template <typename SocketType>
void NetworkThread<SocketType>::RemoveSocket(Socket *socket)
{
    std::lock_guard<std::mutex> lock(m_socketLock);
    m_sockets.erase(socket->shared<SocketType>());
}

#endif //GCEMU_NETWORKTHREAD_H
