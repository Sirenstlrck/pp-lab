#include <iostream>

#include "ClientArgs.hpp"
#include "Sockets/UdpSocket.hpp"

using namespace PPNet;

bool gExitRequested = false;

void RequestExit(int) { gExitRequested = true; }

int main(int argc, char **argv)
try
{
    ClientArgs args = ClientArgs::FromShellArgs(argc, argv);
    UdpSocket socket;

    while (!gExitRequested)
    {
        uint8_t message = args.Number();
        int sended = socket.SendTo(&message, sizeof(message), args.ServerAddress());
        std::cout << "Sended " << sended << " bytes\n";
        sleep(args.Number());
    }
}
catch (std::exception &err)
{
    std::cerr << "An error occured: " << err.what() << '\n';
    return 1;
}
