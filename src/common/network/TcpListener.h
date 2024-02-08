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

#ifndef GCEMU_TCPLISTENER_H
#define GCEMU_TCPLISTENER_H

#include <cstdint>
#include <boost/asio.hpp>
#include <thread>
#include "NetworkThread.h"

template <typename SocketType>
class TcpListener
{
public:
    TcpListener(const std::string& address, int32_t port, int32_t workerThreads);
    ~TcpListener();

private:
    void StartAccept();
    void OnAccept(const std::shared_ptr<NetworkThread<SocketType>>& worker, const std::shared_ptr<SocketType>& socket, const boost::system::error_code& ec);

    std::shared_ptr<NetworkThread<SocketType>> SelectWorker();

    boost::asio::io_context m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;

    std::vector<std::shared_ptr<NetworkThread<SocketType>>> m_workerThreads;

    std::thread m_acceptorThread;
};

template <typename SocketType>
TcpListener<SocketType>::TcpListener(const std::string &address, int32_t port, int32_t workerThreads) : m_ioContext(boost::asio::io_context()), m_acceptor(m_ioContext)
{
    m_acceptor = boost::asio::ip::tcp::acceptor(m_ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

    m_workerThreads.reserve(workerThreads);
    for (int32_t i = 0; i < workerThreads; i++)
        m_workerThreads.push_back(std::make_unique<NetworkThread<SocketType>>());

    StartAccept();
    m_acceptorThread = std::thread([this] () { m_ioContext.run(); });
}

template <typename SocketType>
TcpListener<SocketType>::~TcpListener()
{
    m_ioContext.post([this]() { m_acceptor.close(); });
    m_acceptorThread.join();
}

template <typename SocketType>
void TcpListener<SocketType>::StartAccept()
{
    std::shared_ptr<NetworkThread<SocketType>> worker = SelectWorker();
    std::shared_ptr<SocketType> socket = worker->CreateSocket();

    m_acceptor.async_accept(socket->GetAsioSocket(), [this, worker, socket] (const boost::system::error_code& ec)
    {
        this->OnAccept(worker, socket, ec);
    });
}

template <typename SocketType>
void TcpListener<SocketType>::OnAccept(const std::shared_ptr<NetworkThread<SocketType>>& worker, const std::shared_ptr<SocketType>& socket, const boost::system::error_code &ec)
{
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        worker->RemoveSocket(socket.get());
    }
    else
        socket->Open();

    if (m_acceptor.is_open())
        StartAccept();
}

template <typename SocketType>
std::shared_ptr<NetworkThread<SocketType>> TcpListener<SocketType>::SelectWorker()
{
    size_t minIndex = 0;
    size_t minSize = m_workerThreads[minIndex]->Size();

    for (size_t i = 1; i < m_workerThreads.size(); i++)
    {
        const size_t size = m_workerThreads[i]->Size();
        if (size < minSize)
        {
            minSize = size;
            minIndex = i;
        }
    }

    return m_workerThreads[minIndex];
}

#endif //GCEMU_TCPLISTENER_H
