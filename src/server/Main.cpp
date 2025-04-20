#include <algorithm>
#include <csignal>
#include <cstdint>
#include <memory>

#include "NetHelpers.hpp"
#include "Sockets/TcpSocket.hpp"

using namespace PPNet;

bool gExitRequested = false;
constexpr int ReceiveBufferSize = UINT16_MAX;

void RequestExit(int) { gExitRequested = true; }

using Message = uint8_t;

int main()
try
{
    sighandler_t signalRes = std::signal(SIGINT, RequestExit);
    if (signalRes == SIG_ERR)
        throw std::runtime_error("Failed to set SIGINT handler");

    UdpSocketSPtr listenSocket = std::make_shared<UdpSocket>();

    listenSocket->BindAny();

    sockaddr rawSockaddr = listenSocket->GetSockaddr();
    sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(&rawSockaddr);
    std::cout << "Started listening on port " << ntohs(addr->sin_port) << '\n';

    std::vector<UdpSocketSPtr> readBlockSocket{ listenSocket };
    std::vector<UdpSocketSPtr> readSocket;

    std::array<uint8_t, ReceiveBufferSize> receiveBuffer;

    size_t messageId = 0;
    while (Select(readBlockSocket, readSocket), !gExitRequested)
    {
        for (UdpSocketSPtr &readSocket : readSocket)
        {
            UdpSocket::ReceiveResult receiveResult = readSocket->ReceiveFrom(receiveBuffer.data(), receiveBuffer.size());

            for (int i = 0; i < receiveResult.BytesReceived; ++i)
            {
                Message message = *reinterpret_cast<Message *>(receiveBuffer.data() + i);
                std::cout << '#' <<  messageId << "\t Received " << static_cast<int>(message) << "\n";
            }
            ++messageId;
        }
    }

    return 0;
}
catch (std::exception &err)
{
    std::cerr << "An error occured: " << err.what() << '\n';
    return 1;
}
