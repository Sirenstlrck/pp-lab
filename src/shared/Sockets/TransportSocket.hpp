#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "SocketDescriptor.hpp"

namespace PPNet
{
class TransportSocket
{
protected:
    SocketDescriptor Descriptor;

    TransportSocket(int descriptor) : Descriptor(descriptor) { }

    TransportSocket(SocketDescriptor descriptor) : Descriptor(std::move(descriptor)) { }

public:
    int GetDescriptor() const { return Descriptor.Get(); }

    void SetNonBlockingMode(bool value)
    {
        int flags = fcntl(Descriptor.Get(), F_GETFL, 0);
        if(value)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        fcntl(Descriptor.Get(), F_SETFL, flags);
    }

    sockaddr GetSockaddr()
    {
        sockaddr addr{};
        socklen_t len = sizeof(addr);
        int result = getsockname(Descriptor.Get(), &addr, &len);
        if(result == -1)
            ThrowWithErrnoStr<std::runtime_error>("Failed to call 'getsockname'");

        return addr;
    }

    void BindAny()
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = 0;

        int bindResult = bind(Descriptor.Get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(bindResult == -1)
            ThrowWithErrnoStr<std::runtime_error>("Failed to bind socket");
    }

    int Send(const void* inData, int inLen) const
    {
        int bytesSentCount = send(Descriptor.Get(), static_cast<const char*>(inData), inLen, 0);
        if(bytesSentCount < 0)
            ThrowWithErrnoStr<std::runtime_error>("Socket failed to send data");
        
        return bytesSentCount;
    }

    int Receive(void* inData, int inLen) const
    {
        int bytesReceivedCount = recv(Descriptor.Get(), static_cast<char*>(inData), inLen, 0);
        if(bytesReceivedCount < 0)
            ThrowWithErrnoStr<std::runtime_error>("Socket failed to receive data");
        
        return bytesReceivedCount;
    }

    void Close() { Descriptor.Close(); }
};
}