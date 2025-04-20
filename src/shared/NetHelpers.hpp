#pragma once

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "Sockets/Aliases.hpp"
#include "ArgumentsParsing.hpp"

namespace PPNet
{
static void FillSet(fd_set &set, const std::vector<TcpSocketSPtr> &sockets)
{
    FD_ZERO(&set);
    for (const TcpSocketSPtr &socket : sockets)
    {
        FD_SET(socket->GetDescriptor(), &set);
    }
}

static void FillVectorFromSet(
    fd_set &set,
    const std::vector<TcpSocketSPtr> &full,
    std::vector<TcpSocketSPtr> &destination)
{
    destination.clear();
    for(const TcpSocketSPtr &s : full)
    {
        destination.push_back(s);
    }
}

static int Select(const std::vector<TcpSocketSPtr> &sockets, std::vector<TcpSocketSPtr> &outReadSockets)
{
    fd_set readSet;
    FillSet(readSet, sockets);

    constexpr int Nfds = 1023;
    int selectRes = select(Nfds, &readSet, nullptr, nullptr, nullptr);
    FillVectorFromSet(readSet, sockets, outReadSockets);

    return selectRes;
}
}
