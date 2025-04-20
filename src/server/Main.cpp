#include <cstdint>
#include <csignal>

#include <memory>
#include <algorithm>

#include "Sockets/TcpSocket.hpp"
#include "NetHelpers.hpp"

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


    TcpSocketSPtr listenSocket = std::make_shared<TcpSocket>();

    listenSocket->BindAny();
    
    sockaddr rawSockaddr = listenSocket->GetSockaddr();
    sockaddr_in *addr = reinterpret_cast<sockaddr_in*>(&rawSockaddr);
    std::cout << "Started listening on port " << ntohs(addr->sin_port) << '\n';

    std::vector<TcpSocketSPtr> readBlockSockets { listenSocket };
    std::vector<TcpSocketSPtr> readSockets;
    
    std::array<uint8_t, ReceiveBufferSize> receiveBuffer;
    
    listenSocket->SetNonBlockingMode(true);
    listenSocket->Listen(SOMAXCONN);

    while(Select(readBlockSockets, readSockets), !gExitRequested)
    {
        for(TcpSocketSPtr &readSocket : readSockets)
        {
            if(readSocket == listenSocket)
            {
                std::optional<TcpSocket> newClient = listenSocket->Accept();
                if(newClient.has_value())
                {
                    auto socketPtr = new TcpSocket(std::move(newClient.value()));
                    auto sharedSocketPtr = TcpSocketSPtr(socketPtr);
                    readBlockSockets.push_back(sharedSocketPtr);
                    std::cout << "New client connected\n";
                }
            }
            else
            {
                int bytesReceived = readSocket->Receive(receiveBuffer.data(), receiveBuffer.size());

                // Disconnect
                if(bytesReceived == 0)
                {
                    auto itToErase = std::find(readBlockSockets.begin(), readBlockSockets.end(), readSocket);
                    readBlockSockets.erase(itToErase);
                    std::cout << "Client disconnected\n";
                }
                else
                {
                    for(int i = 0 ; i < bytesReceived; ++i)
                    {
                        Message message = *reinterpret_cast<Message*>(receiveBuffer.data() + i);
                        std::cout << "Received " << (int)message << "\n";
                    }
                }
            }
        }

        readSockets.clear();
    }

    return 0;
}
catch (std::exception &err)
{
    std::cerr << "An error occured: " << err.what() << '\n';
    return 1;
}