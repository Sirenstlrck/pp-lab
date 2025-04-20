#pragma once

#include <netinet/tcp.h>

#include <optional>

#include "TransportSocket.hpp"

namespace PPNet
{

class TcpSocket : public TransportSocket
{
public:
    TcpSocket(int descriptor) : TransportSocket(descriptor) { }
    TcpSocket() : TransportSocket(SocketDescriptor::Open(SOCK_STREAM)) { }

    void SetNoDelay(bool value)
    {
        int yes = value;
        setsockopt(Descriptor.Get(), IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    }

    void Connect(const sockaddr &addr) const
    {
        int err = connect(Descriptor.Get(), &addr, sizeof(addr));
        if(err == -1)
            ThrowWithErrnoStr<std::runtime_error>("TCP socket failed to connect");
    }

    void Listen(int backlog) const
    {
        int err = listen(Descriptor.Get(), backlog);
        if(err == -1)
            ThrowWithErrnoStr<std::runtime_error>("TCP socket failed to listen");
    }

    std::optional<TcpSocket> Accept() const
    {
        sockaddr addr{};
        socklen_t length = sizeof(addr);
        int newSocket = accept(Descriptor.Get(), &addr, &length);
        if(newSocket == -1)
        {
            if(errno == EAGAIN)
                return std::nullopt;
            else
                ThrowWithErrnoStr<std::runtime_error>("Failed to accept socket");
        }

        return TcpSocket(newSocket);
    }
};
}
