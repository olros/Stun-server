#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "Workers.hpp"
#include "stuntypes.h"
#include "ResponseBuilder.hpp"
#include <errno.h>


#define BUFFER_SIZE 256
#define BACKLOG 5
//Added enum for future possibilities of implementing TLS
enum SocketType {
    UDP, TCP
};


//TODO change the client struct to sockaddr_storage to support both ipv4 and ipv6
class Server {
private:
    std::string port;
    Workers *event_loop;
    bool keep_going;
    int socket_fd;
    SocketType socket_type;
    struct addrinfo *result;

    bool init_listening_socket();

    bool handle_udp(ResponseBuilder &builder, sockaddr_in &client, unsigned char (&buffer)[BUFFER_SIZE], socklen_t &length);

    bool handle_tcp(ResponseBuilder builder,struct sockaddr_in client, unsigned char buffer[BUFFER_SIZE], socklen_t length);

public:
    Server();

    Server(std::string socket_port, SocketType sock_type);

    bool startServer();

    void closeServer();

    ~Server();
};

Server::Server() {}

Server::Server(std::string socket_port, SocketType sock_type) {
    this->port = socket_port;
    this->event_loop = new Workers(1);
    this->socket_type = sock_type;
}

bool Server::init_listening_socket() {
    struct addrinfo serverInfo;

    serverInfo.ai_family = AF_INET;
    serverInfo.ai_socktype = socket_type == TCP ? SOCK_STREAM : SOCK_DGRAM;
    serverInfo.ai_flags = AI_PASSIVE;

    memset(&serverInfo, 0, sizeof serverInfo);

    char c_string_port[this->port.length()+1];
    std::strcpy(c_string_port, this->port.c_str());

    int exit_code;
    if ((exit_code = getaddrinfo(NULL, "8080", &serverInfo, &result)) != 0) {
        std::cerr << "getaddrinfo() failed with error code: " << exit_code << std::endl;
        return false;
    }

    //TODO SHOULD I CLOSE HERE AND IN BIND??
    if ((this->socket_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
        std::cerr << "socket() failed with error code: " << socket_fd << std::endl;
        return false;
    }

    if ((exit_code = bind(socket_fd, result->ai_addr, result->ai_addrlen)) == -1) {
        std::cerr << "bind() failed with error code: " << exit_code << std::endl;
        close(this->socket_fd);
        return false;
    }
    return true;
}

bool Server::handle_udp(ResponseBuilder &builder, sockaddr_in &client, unsigned char (&buffer)[BUFFER_SIZE], socklen_t &length) {
    bool isError = false;
    std::cout << "waiting" << std::endl;
    int n = recvfrom(socket_fd, buffer, sizeof(&buffer),
                     MSG_WAITALL, (struct sockaddr *) (&client), &length);
    if (n == -1) {
        std::cerr << "recvfrom() failed: " << strerror(n) << std::endl;
        return false;
    }
    std::cout << "received" << std::endl;
    event_loop->post_after([&client, this, &buffer, &builder, &isError] {
        if (isError || builder.isError())
            sendto(this->socket_fd, builder.buildErrorResponse(400, "Something went wrong!?").getResponse(),
                   sizeof(struct StunErrorResponse), MSG_CONFIRM,
                   (const struct sockaddr *) &client, sizeof(client));

        else
            sendto(this->socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                   MSG_CONFIRM, (const struct sockaddr *) &client, sizeof(client));
        std::cout << "message sent back" << std::endl;
    }, [&builder, &buffer, &client, &isError, &n] {
        builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
        isError = ((buffer[0] >> 6) & 3) != 0 || n < 20;
    });
    return true;
}

bool Server::handle_tcp(ResponseBuilder builder, struct sockaddr_in client, unsigned char buffer[BUFFER_SIZE], socklen_t length) {
    bool isError = false;
    int client_socket_fd = accept(socket_fd, (struct sockaddr *) &client, &length);
    std::cout << "client_socket_fd: " << client_socket_fd << std::endl;
    if (client_socket_fd == -1) return false;
    event_loop->post_after([&builder, &client_socket_fd, &isError]{
        if (isError || builder.isError())
            send(client_socket_fd, builder.buildErrorResponse(400, "Something went wrong!?").getResponse(),
                   sizeof(struct StunErrorResponse), MSG_CONFIRM);
        else
            send(client_socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                   MSG_CONFIRM);
    }, [&builder, &client_socket_fd, &buffer, &isError, client]{
        int n = recv(client_socket_fd, buffer, BUFFER_SIZE,0);
        if(n == -1) std::cerr << "recv() failed: " << n << std::endl;
        builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
        isError = ((buffer[0] >> 6) & 3) != 0 || n < 20;
    });

    close(client_socket_fd);
    return true;
}

//NB! Max 5 connections per socket, when using listen method
//NB! Implement solutions that don differentiate between ipv6 and ipv4, maybe one thread for each, and take in the protocol in constructor
bool Server::startServer() {
    event_loop->start();
    keep_going = true;

    if (!init_listening_socket()) return false;
    if (socket_type == TCP) listen(socket_fd, BACKLOG);
    while (keep_going) {
        //TODO change to sockaddr_storage to support both ipv4 and ipv6
        struct sockaddr_in client;
        unsigned char buffer[BUFFER_SIZE];
        socklen_t length = sizeof(client);
        ResponseBuilder builder;

        //Should not matter if we send copies because we will not manipulate parameters here anymore
        socket_type == TCP ? handle_tcp(builder, client, buffer, length) : handle_udp(builder, client,
                                                                                                  buffer, length);
        char ip4[16];
        inet_ntop(AF_INET, &(((const struct sockaddr_in *)&client)->sin_addr), ip4, sizeof(ip4));
    }
    return true;
}

void Server::closeServer() {
    event_loop->stop();
    event_loop->join();
    keep_going = false;
    freeaddrinfo(result);
    close(socket_fd);
}

Server::~Server() {
    delete event_loop;
}








