#pragma once
#include <cstdint>
#include <stdexcept>

#include <arpa/inet.h>
#include <array>
#include <cstring>

#include "ArgumentsParsing.hpp"
#include "ThrowHelpers.hpp"

namespace PPNet
{
class ClientArgs
{
    uint32_t address_;
    uint16_t port_;
    
    int number_;
    
    public:
    ClientArgs(int argc, char **argv)
    {
        if(argc != 4)
        {
            throw std::invalid_argument("There should be 3 arguments: <address> <port> <number>");
        }
        
        in_addr addr{};
        int ptonResult = inet_pton(AF_INET, argv[1], &addr);
        if (ptonResult <= 0) 
            throw std::runtime_error("Failed to parse address string");

        address_ = addr.s_addr;
        port_ = StoiWithErrorPrefix(argv[2], "Failed to parse port");
        number_ = StoiWithErrorPrefix(argv[3], "Failed to parse number");
        if(number_ < 0 || number_ > 10)
            throw std::runtime_error("The number should be in range [0..10]");
    }

    uint32_t NetOrderAddress() const { return htonl(address_); }
    uint16_t NetOrderPort() const { return htons(port_); }
    
    uint32_t Address() const { return address_; }
    uint16_t Port() const { return port_; }
    int Number() const { return number_; }
};
}