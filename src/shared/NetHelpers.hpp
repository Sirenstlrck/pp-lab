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


template <typename Socket>
static void FillSet(fd_set &set, const std::vector<std::shared_ptr<Socket>> &sockets)
{
    FD_ZERO(&set);
    for (const std::shared_ptr<Socket> &socket : sockets)
    {
        FD_SET(socket->GetDescriptor(), &set);
    }
}

template <typename Socket>
static void FillVectorFromSet(
    fd_set &set,
    const std::vector<std::shared_ptr<Socket>> &full,
    std::vector<std::shared_ptr<Socket>> &destination)
{
    destination.clear();
    for(const std::shared_ptr<Socket> &s : full)
    {
        destination.push_back(s);
    }
}

template<typename Socket>
static int Select(
    const std::vector<std::shared_ptr<Socket>> &sockets,
    std::vector<std::shared_ptr<Socket>> &outReadSockets)
{
    fd_set readSet;
    FillSet(readSet, sockets);

    constexpr int Nfds = 1023;
    int selectRes = select(Nfds, &readSet, nullptr, nullptr, nullptr);
    FillVectorFromSet(readSet, sockets, outReadSockets);

    return selectRes;
}
}
