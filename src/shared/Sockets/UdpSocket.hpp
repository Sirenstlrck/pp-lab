#pragma once

#include "TransportSocket.hpp"
#include "IPv4Address.hpp"

namespace PPNet
{


class UdpSocket : public TransportSocket
{
public:
    struct ReceiveResult
    {
        int BytesReceived;
        IPv4Address From;
    };

    UdpSocket() : TransportSocket(SocketDescriptor::Open(SOCK_DGRAM)) {}

    int SendTo(const void *data, int dataLength, IPv4Address to)
    {
        sockaddr sockaddr = to.AsSockaddr();
        int bytesSent = sendto(Descriptor.Get(), data, dataLength, 0, &sockaddr, IPv4Address::Size);
        if(bytesSent < 0)
            ThrowWithErrnoStr<std::runtime_error>("UDP socket failed to send");
        return bytesSent;
    }

    ReceiveResult ReceiveFrom(void *buffer, int bufferLength)
    {
        socklen_t length = IPv4Address::Size;
        sockaddr sockaddr;
        int read = recvfrom(Descriptor.Get(), buffer, bufferLength, 0, &sockaddr, &length);
        if(read < 0)
            ThrowWithErrnoStr<std::runtime_error>("UDP socket failed to read");
        
        return ReceiveResult{
            .BytesReceived = read,
            .From = IPv4Address::FromSockaddr(sockaddr),
        };
    }
};
} // namespace PPNet
