#pragma once

#include <netinet/tcp.h>

#include <array>
#include <optional>

#include "TransportSocket.hpp"

namespace PPNet
{
class TcpSocket : public TransportSocket
{
public:
    TcpSocket(int descriptor) : TransportSocket(descriptor) {}
    TcpSocket() : TransportSocket(SocketDescriptor::Open(SOCK_STREAM)) {}

    void SetNoDelay(bool value)
    {
        int yes = value;
        setsockopt(Descriptor.Get(), IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    }

    void Connect(const sockaddr &addr) const
    {
        int err = connect(Descriptor.Get(), &addr, sizeof(addr));
        if (err == -1)
            ThrowWithErrnoStr<std::runtime_error>("TCP socket failed to connect");
    }

    void Listen(int backlog) const
    {
        int err = listen(Descriptor.Get(), backlog);
        if (err == -1)
            ThrowWithErrnoStr<std::runtime_error>("TCP socket failed to listen");
    }

    std::optional<TcpSocket> Accept() const
    {
        sockaddr addr{};
        socklen_t length = sizeof(addr);
        int newSocket = accept(Descriptor.Get(), &addr, &length);
        if (newSocket == -1)
        {
            if (errno == EAGAIN)
                return std::nullopt;
            else
                ThrowWithErrnoStr<std::runtime_error>("Failed to accept socket");
        }

        return TcpSocket(newSocket);
    }

    int Send(const void *inData, int inLen) const
    {
        int bytesSentCount = send(Descriptor.Get(), static_cast<const char *>(inData), inLen, 0);
        if (bytesSentCount < 0)
            ThrowWithErrnoStr<std::runtime_error>("Socket failed to send data");

        return bytesSentCount;
    }

    int Receive(void *inData, int inLen) const
    {
        int bytesReceivedCount = recv(Descriptor.Get(), static_cast<char *>(inData), inLen, 0);
        if (bytesReceivedCount < 0)
            ThrowWithErrnoStr<std::runtime_error>("Socket failed to receive data");

        return bytesReceivedCount;
    }
};
} // namespace PPNet
