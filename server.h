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
#define MAX_EVENTS 10

// Forward declaration of the Server struct
struct Server;

// Function prototype
void sendTime(int client_socket);
int setNonBlocking(int fd);

// Struct definition
struct Server {
  int domain;
  int port;
  int service;
  int protocol;
  int backlog;
  u_long interface;

  int num_threads;

  int socket;
  struct sockaddr_in address;

  void (*launch)(struct Server *server);
};

#endif //SERVER_H