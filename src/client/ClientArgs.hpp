#pragma once
#include <cstdint>
#include <stdexcept>

#include <arpa/inet.h>
#include <array>
#include <cstring>

#include "ArgumentsParsing.hpp"
#include "Sockets/IPv4Address.hpp"
#include "ThrowHelpers.hpp"

namespace PPNet
{
class ClientArgs
{
    IPv4Address serverAddress_;

    int number_;

public:
    static ClientArgs FromShellArgs(int argc, char **argv)
    {
        if (argc != 4)
        {
            throw std::invalid_argument("There should be 3 arguments: <address> <port> <number>");
        }

        uint16_t port = StoiWithErrorPrefix(argv[2], "Failed to parse port");
        IPv4Address serverAddress = IPv4Address::FromPresentation(argv[1], port);
        int number = StoiWithErrorPrefix(argv[3], "Failed to parse number");

        return ClientArgs(serverAddress, number);
    }

    ClientArgs(IPv4Address serverAddress, int number)
        : serverAddress_(serverAddress), number_(number)
    {
        if (number_ < 0 || number_ > 10)
            throw std::runtime_error("The number should be in range [0..10]");
    }

    IPv4Address ServerAddress() const { return serverAddress_; }
    int Number() const { return number_; }
};
} // namespace PPNet
