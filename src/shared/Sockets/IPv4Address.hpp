#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>

namespace PPNet
{
class IPv4Address
{
public:
    static constexpr size_t Size = sizeof(sockaddr_in);

    static uint32_t PtoN(const char *pstr)
    {
        in_addr addr{};
        int result = inet_pton(AF_INET, pstr, &addr);
        if (result <= 0)
            throw std::runtime_error("Failed to parse address string");

        return static_cast<uint32_t>(addr.s_addr);
    }

    static IPv4Address FromPresentation(const char *addresStr, uint16_t hostOrderPort)
    {
        uint32_t address = PtoN(addresStr);
        uint32_t port = htons(hostOrderPort);
        return IPv4Address(address, port);
    }

    static IPv4Address FromSockaddr(sockaddr sockaddr)
    {
        sockaddr_in *sockaddrIn = reinterpret_cast<sockaddr_in*>(&sockaddr);
        return IPv4Address(*sockaddrIn);
    }

private:
    sockaddr_in addr_;

    IPv4Address(uint32_t netOrderAddress, uint16_t netOrderPort)
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = netOrderAddress;
        addr.sin_port = netOrderPort;

        addr_ = addr;
    }

    IPv4Address(sockaddr_in sockaddr) : addr_(sockaddr) { }

public:

    sockaddr_in AsSockaddrIn() const { return addr_; }

    sockaddr AsSockaddr() const { return *reinterpret_cast<const sockaddr *>(&addr_); }
};
} // namespace PPNet
