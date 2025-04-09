//
//    test.c
//    Created by user1 on 06.04.25.
//

#include <stdio.h>
#include "Server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <string.h>

void launch(struct Server *server) { //needs pointer to server
  char buffer[30000];
  printf("Starting server and waiting for connection...\n");

  socklen_t adress_length = sizeof(server->address);
  int new_socket = accept(server->socket, (struct sockaddr *)&server->address, &adress_length);
  read(new_socket, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  char *hello = "HTTP/1.1 200 OK\nDate: Wed, 09 Apr 2025 11:26:15 GMT\nServer: Apache/2.2.14 (Linux)\nLast-Modified: Wed, 09 Apr 2025 11.27.15 GMT\nContent-Length: 88\nContent-Type: text(html\nConnection: Closed\n<html><body><h1>Hello OS-Group!</h1></body></html>";
  write(new_socket, hello, strlen(hello));
  close(new_socket);
}


int main() {
  struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8097, 10, NULL);

  if (server.socket < 0) {
    fprintf(stderr, "Failed to create server socket\n");
    return 1;
  }
  printf("2\n");
  launch(&server);

  close(server.socket); // Cleanup server socket

  return 0;
}