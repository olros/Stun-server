#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Workers.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include "stuntypes.h"
#include "ResponseBuilder.hpp"

class Server {
private:
    int port;
    Workers *event_loop;
    //No use for atomic, worst case scenario the server receives another request
    bool keep_going;
public:
    Server();

    Server(int port);

    bool startServer();

    void closeServer();

    ~Server();
};

Server::Server() {}

Server::Server(int port) {
    this->port = port;
    this->event_loop = new Workers(1);
}

//NB! Max 5 connections per socket, when using listen method
//NB! Implement sultions that don differentiate between ipv6 and ipv4, maybe one thread for each, and take in the protocol in constructor
bool Server::startServer() {
    //May need to add variables to the post method in event_loop in c++
    event_loop->start();
    keep_going = true;
    struct sockaddr_in server;
    struct sockaddr_in6 server_ip6;
    const int bufferSize = 256;
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) return false;

    //Ipv6
    // server_ip6.sin6_family=AF_INET6;
    // server_ip6.sin6_port=htons(this->port);
    // server_ip6.sin6_addr=in6addr_any;
    //Ipv4
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;


    if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) {
        close(socket_fd);
        return false;
    }

    while (keep_going) {
        //Have to create a new client ipv6 address for each run of the loop
        struct sockaddr_in6 client_ipv6;
        struct sockaddr_in client;
        socklen_t length = sizeof(client);
        unsigned char buffer[bufferSize];



        //TODO add a mutex to the socket so that only one thread at a time can use it
        //TODO passing variables tot the event_loop is not a problem since we always can pass it's address instead ensurig that the change can be viewed elsewhere


        //Dont need to lock the receiving part of the socket since it is full duplex, and only one thread is reading
        recvfrom(socket_fd, buffer, bufferSize, MSG_WAITALL, (struct sockaddr *) &client,
                     &length);
        ResponseBuilder builder;
        //Passing the address of the variables since we don't want a copy
        event_loop->post_after([ &server, &client, &socket_fd, &buffer, &builder] {
            //Dont have to lock sending from the socket since it is full duplex, and it is running in an event loop
            //2048 is equal to MSG_CONFIRM which the IDE does not recognize
            sendto(socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                   2048, (const struct sockaddr *) &client, sizeof(client));
        }, [&builder, &buffer, &client] {
            builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
        });

        //TODO remove logging of ipv4 and ipv6 addresses
        char ip4[16];
        inet_ntop(AF_INET, &client.sin_addr, ip4, sizeof(ip4));

        std::cout << "v4: " << ip4 << " : " << htons(client.sin_addr.s_addr) << " : " << ntohs(client.sin_port) << std::endl;
        std::cout << "New request, yey"  << std::endl;


        // char ip6[16];
        // inet_ntop(AF_INET6, &client_ipv6.sin6_addr, ip6, sizeof(ip6));
        // std::cout << "v6: " << ip6 << " : " << ntohs(client_ipv6.sin6_port) << std::endl;
    }
    close(socket_fd);
    return true;
}

void Server::closeServer() {
    event_loop->stop();
    event_loop->join();
    keep_going = false;
}

Server::~Server() {
    delete event_loop;
}








