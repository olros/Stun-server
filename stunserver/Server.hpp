#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Workers.hpp"
#include "stuntypes.h"
#include "ResponseBuilder.hpp"

//Added enum for future possibilities of implementing TLS
enum SocketType {
    UDP, TCP
};

class Server {
private:
    char *port;
    Workers *event_loop;
    bool keep_going;
    int socket_fd;
    const int BUFFER_SIZE = 256;
    SocketType sock_type;
    const int BACKLOG = 5;

    bool init_receive_socket();

    bool handle_udp();

    bool handle_tcp();

public:
    Server();

    Server(char *port, SocketType sock_type);

    bool startServer();

    void closeServer();

    ~Server();
};

Server::Server() {}

Server::Server(char *port, SocketType sock_type) {
    this->port = port;
    this->event_loop = new Workers(1);
    this->sock_type = sock_type;
}

bool Server::init_receive_socket() {
    struct addrinfo serverInfo, *result;

    serverInfo.ai_family = AF_INET;
    serverInfo.ai_socktype = SOCK_DGRAM;
    serverInfo.ai_flags = AI_PASSIVE;

    int exit_code;
    if ((exit_code = getaddrinfo(NULL, port, &serverInfo, &result)) != 0) {
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

bool Server::handle_udp(ResponseBuilder builder, struct sockaddr_storage client, unsigned char buffer[BUFFER_SIZE], socklen_t length, bool isError) {
    int n = recvfrom(socket_fd, buffer, sizeof(buffer),
                     MSG_WAITALL, (struct sockaddr *) (&client), &length);
    event_loop->post_after([&client, &socket_fd, &buffer, &builder, &isError] {
        if (isError || builder.isError())
            sendto(socket_fd, builder.buildErrorResponse(400, "Something went wrong!?").getResponse(),
                   sizeof(struct StunErrorResponse), MSG_CONFIRM,
                   (const struct sockaddr *) &client, sizeof(client));

        else
            sendto(socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                   MSG_CONFIRM, (const struct sockaddr *) &client, sizeof(client));

    }, [&builder, &buffer, &client, &isError, &n] {
        builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
        isError = ((buffer[0] >> 6) & 3) != 0 || n < 20;
    });
}

bool Server::handle_tcp(ResponseBuilder builder, struct sockaddr_storage client, unsigned char buffer[BUFFER_SIZE], socklen_t length, bool isError) {
    int client_socket_fd = accept(socket_fd, (struct sockaddr *) &client, &length);
    if (client_socket_fd == -1) return false;
    event_loop->post_after([&builder, &client_socket_fd]{
        if (isError || builder.isError())
            send(client_socket_fd, builder.buildErrorResponse(400, "Something went wrong!?").getResponse(),
                   sizeof(struct StunErrorResponse), MSG_CONFIRM);
        else
            send(client_socket_fd, builder.buildSuccessResponse().getResponse(), sizeof(struct STUNResponseIPV4),
                   MSG_CONFIRM);
    }, [&builder, &client_socket_fd, &buffer, &BUFFER_SIZE]{
        int n = recv(client_socket_fd, buffer, BUFFER_SIZE,0);
        builder = ResponseBuilder(true, (STUNIncommingHeader *) buffer, client);
        isError = ((buffer[0] >> 6) & 3) != 0 || n < 20;
    });

    close(client_socket_fd);
    return true;
}

//NB! Max 5 connections per socket, when using listen method
//NB! Implement sultions that don differentiate between ipv6 and ipv4, maybe one thread for each, and take in the protocol in constructor
bool Server::startServer() {
    event_loop->start();
    keep_going = true;

    if (!init_receive_socket()) return false;
    if (sock_type == TCP) listen(socket_fd, BACKLOG);
    while (keep_going) {
        struct sockaddr_storage client;
        unsigned char buffer[bufferSize];
        socklen_t length = sizeof(client);
        ResponseBuilder builder;
        bool isError = false;

        sock_type == TCP ? handle_tcp(&builder, &client, &buffer, &length, &isError) : handle_udp(&builder, &client,
                                                                                                  &buffer, &length,
                                                                                                  &isError);
        char ip4[16];
        inet_ntop(AF_INET, &client.sin_addr, ip4, sizeof(ip4));
    }
    return true;
}

void Server::closeServer() {
    event_loop->stop();
    event_loop->join();
    keep_going = false;
    close(socket_fd);
}

Server::~Server() {
    delete event_loop;
}








