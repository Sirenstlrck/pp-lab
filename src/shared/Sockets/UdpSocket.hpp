#pragma once

#include "TransportSocket.hpp"

namespace PPNet
{
class UdpSocket : public TransportSocket
{
public:
    UdpSocket() : TransportSocket(SocketDescriptor::Open(SOCK_DGRAM)) { }
};
}
