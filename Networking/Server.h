//
//    Server.h
//    Created by user1 on 06.04.25.
//

#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h> // for u_long

struct Server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address; // for Adress

    int socket;

    void(*launch)(void); // takes no parameters

};

// Function prototype, defines function without instanciate it
struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog,
                                 void(*launch)(void));

#endif //SERVER_H
