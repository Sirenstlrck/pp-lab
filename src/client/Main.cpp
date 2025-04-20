#include <iostream>

#include "ClientArgs.hpp"
#include "Sockets/TcpSocket.hpp"

using namespace PPNet;

bool gExitRequested = false;

void RequestExit(int) { gExitRequested = true; }

int main(int argc, char **argv)
try
{
    ClientArgs args(argc, argv);
    TcpSocket socket;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = args.Address();
    addr.sin_port = args.NetOrderPort();
    sockaddr *rawAddr = reinterpret_cast<sockaddr*>(&addr);
    socket.SetNoDelay(true);
    socket.Connect(*rawAddr);

    while(!gExitRequested)
    {
        uint8_t message = args.Number();
        socket.Send(&message, sizeof(message));
        sleep(args.Number());
    }
}
catch (std::exception &err)
{
    std::cerr << "An error occured: " << err.what() << '\n';
    return 1;
}
