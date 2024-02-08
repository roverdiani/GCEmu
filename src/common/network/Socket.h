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

#ifndef GCEMU_SOCKET_H
#define GCEMU_SOCKET_H

#include <mutex>
#include <boost/asio.hpp>
#include "PacketBuffer.h"

class Socket : public std::enable_shared_from_this<Socket>
{
public:
    Socket(boost::asio::io_context& ioContext, const std::function<void (Socket*)>& closeHandler);

    virtual bool Open();
    void Close();

    bool IsClosed() const;

    boost::asio::ip::tcp::socket& GetAsioSocket();

    bool Read(char* buffer, int length);
    void Write(const char* buffer, int32_t length);

    template <typename T>
    std::shared_ptr<T> shared() { return std::static_pointer_cast<T>(shared_from_this()); }

protected:
    virtual bool ProcessIncomingData() = 0;
    size_t ReadLengthRemaining() const;

    std::string m_address;
    std::string m_remoteEndpoint;
    boost::asio::ip::address m_remoteAddress;
    uint16_t m_remotePort = 0;

private:
    void StartAsyncRead();
    void OnRead(const boost::system::error_code& ec, size_t length);
    void OnWriteComplete(const boost::system::error_code& ec, size_t length);

    boost::asio::ip::tcp::socket m_socket;

    std::mutex m_closeLock;

    std::function<void(Socket*)> m_closeHandler;

    std::unique_ptr<PacketBuffer> m_inBuffer;
    std::unique_ptr<PacketBuffer> m_outBuffer;

    // custom allocator based on example from http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/example/cpp11/allocation/server.cpp
    // Class to manage the memory to be used for handler-based custom allocation.
    // It contains a single block of memory which may be returned for allocation
    // requests. If the memory is in use when an allocation request is made, the
    // allocator delegates allocation to the global heap.
    class handler_allocator
    {
    private:
        static const size_t BufferSize = 512;

        // Storage space used for handler-based custom memory allocation.
        std::aligned_storage<BufferSize>::type m_buffer;

        // Whether the handler-based custom allocation storage has been used.
        bool m_inUse;

    public:
        handler_allocator() : m_inUse(false) {}

        handler_allocator(const handler_allocator&) = delete;
        handler_allocator& operator=(const handler_allocator&) = delete;

        void* allocate(std::size_t size)
        {
            if (!m_inUse && size <= sizeof(m_buffer))
            {
                m_inUse = true;
                return &m_buffer;
            }

            return ::operator new(size);
        }

        void deallocate(void* pointer)
        {
            if (pointer == &m_buffer)
                m_inUse = false;
            else
                ::operator delete(pointer);
        }
    };

    // Wrapper class template for handler objects to allow handler memory
    // allocation to be customised. Calls to operator() are forwarded to the
    // encapsulated handler.
    template <typename Handler>
    class custom_alloc_handler
    {
    private:
        handler_allocator& m_allocator;
        Handler m_handler;

    public:
        custom_alloc_handler(handler_allocator& a, Handler h) : m_allocator(a), m_handler(h) {}

        template <typename ...Args>
        void operator()(Args&&... args)
        {
            m_handler(std::forward<Args>(args)...);
        }

        friend void* asio_handler_allocate(std::size_t size, custom_alloc_handler<Handler>* this_handler)
        {
            return this_handler->m_allocator.allocate(size);
        }

        friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/, custom_alloc_handler<Handler>* this_handler)
        {
            this_handler->m_allocator.deallocate(pointer);
        }
    };

    // Helper function to wrap a handler object to add custom allocation.
    template <typename Handler>
    static custom_alloc_handler<Handler> make_custom_alloc_handler(handler_allocator& a, Handler h)
    {
        return custom_alloc_handler<Handler>(a, h);
    }

    handler_allocator m_allocator;
};

#endif //GCEMU_SOCKET_H
