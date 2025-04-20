#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>
#include <string>

#include "ThrowHelpers.hpp"

namespace PPNet
{
constexpr int InvalidSocket = -1;

class SocketDescriptor
{
    int descriptor_;

public:
    SocketDescriptor(int descriptor) : descriptor_(descriptor){};

    static SocketDescriptor Open(int type)
    {
        int descriptor = socket(AF_INET, type, 0);
        if (descriptor < 0)
            ThrowWithErrnoStr<std::runtime_error>("Failed to create socket");

        return SocketDescriptor(descriptor);
    }

    int Get() const { return descriptor_; }

    SocketDescriptor(const SocketDescriptor &rhs) = delete;
    SocketDescriptor &operator=(const SocketDescriptor &rhs) = delete;

    SocketDescriptor(SocketDescriptor &&rhs)
    {
        if (this == &rhs)
            return;

        descriptor_ = rhs.descriptor_;
        rhs.descriptor_ = InvalidSocket;
    }

    SocketDescriptor &operator=(SocketDescriptor &&rhs)
    {
        std::swap(descriptor_, rhs.descriptor_);
        return *this;
    }

    void Close()
    {
        int closeResult = close(descriptor_);
        if (closeResult == -1)
            ThrowWithErrnoStr<std::runtime_error>("Failed to close socket");

        descriptor_ = InvalidSocket;
    }

    ~SocketDescriptor()
    {
        if (descriptor_ == InvalidSocket)
            return;
        Close();
    }
};
} // namespace PPNet
