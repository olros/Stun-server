#include <iostream>
#include "Server.hpp"
#include <thread>

int main() {
    std::cout << "The server is starting, hang on a second..." << std::endl;
    Server server(80);
    std::cout << server.startServer() << std::endl;
    std::cout << "Stun server started!" << std::endl;
    return 0;
}
