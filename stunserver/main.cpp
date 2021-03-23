#include <iostream>
#include "Server.hpp"
#include <thread>

#define SOCKET_PORT 3478
#define TLS_PORT 5349

int main() {
    std::cout << "The server is starting, hang on a second..." << std::endl;
    Server tcp_server(SOCKET_PORT, SocketType::TCP);
    Server udp_server(SOCKET_PORT, SocketType::UDP);
    Server tls_server(TLS_PORT, SocketType::TLS);
    std::thread tcp([&tcp_server] {
        tcp_server.startServer();
    });
    std::thread udp([&udp_server] {
        udp_server.startServer();
    });
    std::thread tls([&tls_server] {
        tls_server.startServer();
    });
    std::cout << "Stun server started!" << std::endl;

    tcp.join();
    udp.join();
    tls.join();
    return 0;
}
