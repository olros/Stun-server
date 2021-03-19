#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Workers.hpp"
#include "stuntypes.h"
#include "ResponseBuilder.hpp"

class Server {
private:
    int port;
    Workers *event_loop;
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
        std::cout <<"uuuu"<<std::endl;
        struct sockaddr_in6 client_ipv6;
        struct sockaddr_in client;
        socklen_t length = sizeof(client);
        unsigned char buffer[bufferSize];
        int n = recvfrom(socket_fd, buffer, sizeof(buffer), 
        MSG_WAITALL, (struct sockaddr*)(&client),&length);
        ResponseBuilder builder;
        bool isError = false;
        event_loop->post_after([ &server, &client, &socket_fd, &buffer, &builder, &isError] {
            if(isError || builder.isError()) sendto(socket_fd, builder.buildErrorResponse(400, "Something went wrong!?").getResponse(),sizeof(struct StunErrorResponse), MSG_CONFIRM, 
                (const struct sockaddr *) &client, sizeof(client));

            else sendto(socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                       MSG_CONFIRM, (const struct sockaddr *) &client, sizeof(client));
    
        }, [&builder, &buffer, &client, &isError, &n] {
            builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
            isError =((buffer[0] >> 6) & 3) != 0 || n<20;
        });

        char ip4[16];
        inet_ntop(AF_INET, &client.sin_addr, ip4, sizeof(ip4));
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








