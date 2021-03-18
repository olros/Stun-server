#include <iostream>
#include "Server.hpp"
#include <thread>


int main() {
    std::cout << "The server is starting, hang on a second..." << std::endl;

    Server server(80);
    std::cout << server.startServer() << std::endl;
    std::cout << "Stun server started!" << std::endl;

    /*
    std::thread server_thread = std::thread([&server] {

    });


    //NB! Add thread here to make the server be able to close gracefully
    //Have to use address so that we dont copy server
    //Necessary since a server is just supposed to run anyways
    //If cin does not poll this does not take processing power, but if cin poll we lose a lot of processing power from the endless loop
    std::thread end = std::thread([&server] {
        bool keep_going = true;
        while (keep_going) {
            std::cout << "Enter e to exit the server." << std::endl;
            std::string s;
            std::cin >> s;
            if (s[0] == 'e') {
                std::cout << "Server shutting down..." << std::endl;
                keep_going = false;
                server->closeServer();
                std::cout << "Server shut down!" << std::endl;
            }
        }
    });

    server_thread.join();
    end.join();*/
    return 0;
}
