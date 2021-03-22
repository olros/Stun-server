#include <iostream>
#include "Server.hpp"
#include <thread>

#define SOCKET_PORT 3478
int main() {
    std::cout << "The server is starting, hang on a second..." << std::endl;
    Server tcp_server(SOCKET_PORT, SocketType::TCP);
    Server udp_server(SOCKET_PORT, SocketType::UDP);
    std::thread tcp([&tcp_server]{
        tcp_server.startServer();
    });
    std::thread udp([&udp_server]{
        udp_server.startServer();
    });
    std::cout << "Stun server started!" << std::endl;

    tcp.join();
    udp.join();
    return 0;
}
