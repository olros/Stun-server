#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bitset>
#include "Workers.hpp"
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


    if(bind(socket_fd, (struct sockaddr*)(&server), sizeof(server)) < 0){
        close(socket_fd);
        return false;
    }

    while (keep_going) {
        //Have to create a new client ipv6 address for each run of the loop
        struct sockaddr_in6 client_ipv6;
        struct sockaddr_in client;
        socklen_t length = sizeof(client);
        unsigned char buffer[bufferSize];
        int n = recvfrom(socket_fd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)(&client),
                     &length);
        ResponseBuilder builder;
        bool isError = false;
        std::cout << buffer << std::endl;

        //Passing the address of the variables since we don't want a copy
        event_loop->post_after([ &server, &client, &socket_fd, &buffer, &builder, &isError] {
            //Dont have to lock sending from the socket since it is full duplex, and it is running in an event loop
            //2048 is equal to MSG_CONFIRM which the IDE does not recognize
            std::cout << "isError: " << isError << std::endl;
            if(isError) {
                //TODO send better error message
                sendto(socket_fd, builder.buildErrorResponse().getResponse(),sizeof(struct StunErrorResponse), 2048, (const struct sockaddr *) &client, sizeof(client));
            } else{
                sendto(socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                       2048, (const struct sockaddr *) &client, sizeof(client));
            }
            //sendto(socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
             //     2048, (const struct sockaddr *) &client, sizeof(client));
        }, [&builder, &buffer, &client, &isError, &n] {
            std::cout << "n: " << n << std::endl;
            //TODO remove logging of error detection
            builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
            isError =((buffer[0] >> 6) & 3) != 0 || n<20;
        });

        //TODO remove logging of ipv4 and ipv6 addresses
        char ip4[16];
        inet_ntop(AF_INET, &client.sin_addr, ip4, sizeof(ip4));

        std::cout << "New request:"  << std::endl;
        std::cout << "v4: " << ip4 << " : " << htons(client.sin_addr.s_addr) << " : " << ntohs(client.sin_port) << std::endl;


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








