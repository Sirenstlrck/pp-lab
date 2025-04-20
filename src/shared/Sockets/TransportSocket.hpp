#pragma once

#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include "SocketDescriptor.hpp"

namespace PPNet
{
class TransportSocket
{
protected:
    SocketDescriptor Descriptor;

    TransportSocket(int descriptor) : Descriptor(descriptor) {}

    TransportSocket(SocketDescriptor descriptor) : Descriptor(std::move(descriptor)) {}

public:
    int GetDescriptor() const { return Descriptor.Get(); }

    void SetNonBlockingMode(bool value)
    {
        int flags = fcntl(Descriptor.Get(), F_GETFL, 0);
        if (value)
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
        if (result == -1)
            ThrowWithErrnoStr<std::runtime_error>("Failed to call 'getsockname'");

        return addr;
    }

    void BindAny()
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = 0;

        int bindResult = bind(Descriptor.Get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
        if (bindResult == -1)
            ThrowWithErrnoStr<std::runtime_error>("Failed to bind socket");
    }


    void Close() { Descriptor.Close(); }
};
} // namespace PPNet
