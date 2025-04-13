//
// Created by user1 on 13.04.25.
//

#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/types.h>

// Constants
#define PORT 8080
#define BUFFER_SIZE 16000

// Forward declaration of the Server struct
struct Server;

// Function prototype
void send_time(int client_socket);

// Struct definition
struct Server {
  int domain;
  int port;
  int service;
  int protocol;
  int backlog;
  u_long interface;

  int socket;
  struct sockaddr_in address;

  void (*launch)(struct Server *server);
};

#endif //SERVER_H