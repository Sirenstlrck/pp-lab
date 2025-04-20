#pragma once

#include <memory>

#include "TcpSocket.hpp"
#include "UdpSocket.hpp"

namespace PPNet
{
using TcpSocketSPtr = std::shared_ptr<TcpSocket>;
using UdpSocketSPtr = std::shared_ptr<UdpSocket>;
}
