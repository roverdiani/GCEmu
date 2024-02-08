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

#include "Socket.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <memory>

Socket::Socket(boost::asio::io_context &ioContext, const std::function<void(Socket *)>& closeHandler) : m_socket(ioContext)
{
}

bool Socket::Open()
{
    try
    {
        m_address = m_socket.remote_endpoint().address().to_string();
        m_remoteEndpoint = boost::lexical_cast<std::string>(m_socket.remote_endpoint());
        m_remoteAddress = m_socket.remote_endpoint().address();
        m_remotePort = m_socket.remote_endpoint().port();
    }
    catch (boost::system::system_error& error)
    {
        std::cout << "Socket::Open() failed to get remote address. Error: %s", error.what();
        return false;
    }

    m_outBuffer = std::make_unique<PacketBuffer>();
    m_inBuffer = std::make_unique<PacketBuffer>();

    StartAsyncRead();

    return true;
}

void Socket::Close()
{
    std::lock_guard<std::mutex> lock(m_closeLock);
    if (IsClosed())
        return;

    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close();

    if (m_closeHandler)
        m_closeHandler(this);
}

bool Socket::IsClosed() const
{
    return !m_socket.is_open();
}

boost::asio::ip::tcp::socket &Socket::GetAsioSocket()
{
    return m_socket;
}

void Socket::StartAsyncRead()
{
    if (IsClosed())
        return;

    std::shared_ptr<Socket> ptr = shared<Socket>();
    m_socket.async_read_some(boost::asio::buffer(&m_inBuffer->m_buffer[m_inBuffer->m_writePosition], m_inBuffer->m_buffer.size() - m_inBuffer->m_writePosition),
                             make_custom_alloc_handler(m_allocator, [ptr](const boost::system::error_code& ec, size_t length) { ptr->OnRead(ec, length); }));
}

void Socket::OnRead(const boost::system::error_code &ec, size_t length)
{
    if (ec)
        return;

    if (IsClosed())
        return;

    m_inBuffer->m_writePosition += length;

    const size_t available = m_socket.available();
    if (available > 0 && (length + available) > m_inBuffer->m_buffer.size())
    {
        m_inBuffer->m_buffer.resize(m_inBuffer->m_buffer.size() + available);
        StartAsyncRead();
        return;
    }

    while (m_inBuffer->m_readPosition < m_inBuffer->m_writePosition)
    {
        if (ProcessIncomingData())
            continue;

        // This errno is set when there is not enough buffer data available to either complete a header, or the packet length
        // specified in the header goes past what we've read. In this case, we will reset the buffer with the remaining data.
        if (errno == EBADMSG)
        {
            const size_t bytesRemaining = m_inBuffer->m_writePosition - m_inBuffer->m_readPosition;
            std::memmove(&m_inBuffer->m_buffer[0], &m_inBuffer->m_buffer[m_inBuffer->m_readPosition], bytesRemaining);
            m_inBuffer->m_readPosition = 0;
            m_inBuffer->m_writePosition = bytesRemaining;

            StartAsyncRead();
        }
        else if (!IsClosed())
            Close();

        return;
    }

    m_inBuffer->m_writePosition = m_inBuffer->m_readPosition = 0;

    StartAsyncRead();
}

size_t Socket::ReadLengthRemaining() const
{
    return m_inBuffer->ReadLengthRemaining();
}

void Socket::Write(const char *buffer, int32_t length)
{
    PacketBuffer* outBuffer = m_outBuffer.get();

    outBuffer->Write(buffer, length);

    if (IsClosed())
        return;

    std::shared_ptr<Socket> ptr = shared<Socket>();
    m_socket.async_write_some(boost::asio::buffer(m_outBuffer->m_buffer, m_outBuffer->m_writePosition),
                              make_custom_alloc_handler(m_allocator, [ptr](const boost::system::error_code& ec, size_t length)
                              { ptr->OnWriteComplete(ec, length); }));
}

void Socket::OnWriteComplete(const boost::system::error_code &ec, size_t length)
{
    if (ec)
        return;

    if (IsClosed())
        return;

    if (length < m_outBuffer->m_writePosition)
    {
        memcpy(&(m_outBuffer->m_buffer[0]), &(m_outBuffer->m_buffer[length]),
               (m_outBuffer->m_writePosition - length) * sizeof(m_outBuffer->m_buffer[0]));
        m_outBuffer->m_writePosition -= length;
    }
    else
        m_outBuffer->m_writePosition = 0;

    std::shared_ptr<Socket> ptr = shared<Socket>();
    if (m_outBuffer->m_writePosition > 0)
        m_socket.async_write_some(boost::asio::buffer(m_outBuffer->m_buffer, m_outBuffer->m_writePosition),
                                  make_custom_alloc_handler(m_allocator, [ptr](const boost::system::error_code& ec, size_t length)
                                  { ptr->OnWriteComplete(ec, length); }));
}

bool Socket::Read(char *buffer, int length)
{
    if (ReadLengthRemaining() < length)
        return false;

    m_inBuffer->Read(buffer, length);

    return true;
}
