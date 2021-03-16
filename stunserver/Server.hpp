#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Workers.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include "stuntypes.h"
#include "SuccessResponseBuilder.hpp"

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
    struct sockaddr_in server, client;
    struct sockaddr_in6 server_ip6, client_ipv6;
    const int bufferSize =256;
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fd == -1) return false;


    //Ipv6
    // server_ip6.sin6_family=AF_INET6;
    // server_ip6.sin6_port=htons(this->port);
    // server_ip6.sin6_addr=in6addr_any;
    //Ipv4
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;


    if(bind(socket_fd, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)) < 0){
        close(socket_fd);
        return false;
    }

    int n;
    socklen_t length = sizeof(client);

    while (keep_going) {
        unsigned char buffer[256];
        n = recvfrom(socket_fd, buffer, sizeof(buffer), MSG_WAITALL, reinterpret_cast<struct sockaddr*>(&client),
                     &length);
        //TODO remove logging of ipv4 and ipv6 addresses
        // char ip4[16];
        // inet_ntop(AF_INET, &client.sin_addr, ip4, sizeof(ip4));
        // std::cout << "v4: " << ip4 << " : " << ntohs(client.sin_addr.s_addr) << std::endl;

        // char ip6[16];
        // inet_ntop(AF_INET6, &client_ipv6.sin6_addr, ip6, sizeof(ip6));
        // std::cout << "v6: " << ip6 << " : " << ntohs(client_ipv6.sin6_port) << std::endl;

         struct STUNIncommingHeader* inc = (STUNIncommingHeader*) buffer;
        // unsigned char* ipv6addresss = client.sin_addr.s_addr;
        //example on how to make a request 
        SuccessResponseBuilder builder = SuccessResponseBuilder();
        builder.setStunSuccessHeaders(inc);
        builder.setlength(0);
        builder.setAttLength(0);
        builder.setAttType(0);
        builder.setPadding(0);
        builder.setProtocol(true);
        // unsigned char* n = (unsigned char*)client.sin_addr.s_addr;
        sockaddr_in to = client;
        builder.XORAttributes(client.sin_addr.s_addr, client.sin_port, true);
        sendto(socket_fd, builder.getResponse(), sizeof(struct STUNResponse), 2048, (const struct sockaddr *) &to, sizeof(to));
        std::cout<<"ssdds"<<std::endl;
        //use asio
        
        //2048 is equal to MSG_CONFIRM which the IDE does not recognize
    }
    close(socket_fd);
    return true;
}

void Server::closeServer() {
    event_loop->stop();
    event_loop->join();
}

Server::~Server() {
    delete event_loop;
}





