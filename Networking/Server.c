//
//    Sercer.c
//    Created by user1 on 06.04.25.
//

#include "Server.h"
#include <stdio.h>
#include <stdlib.h>

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog,
                                 void(*launch)(void)) {
    struct Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;
    server.launch = launch;

    // Create address
    server.address.sin_family = domain;
    server.address.sin_port = htons(port); // htons converting int port to network port
    server.address.sin_addr.s_addr = htonl(interface);

    // Create socket
    server.socket = socket(domain, service, protocol); // Create socketconnection to network
    // Test socketconnection
    if (server.socket == 0) {
      perror("Failed to connect socket...\n");
      exit(1);
     }

    // Allow address reuse (important for repeated testing)
    int opt = 1;
    if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }

    // Bind socket, included in test (if loop)
    if(bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0) {
       perror("Failed to bind socket...\n");
       exit(1);
    }

    // Listener in test-loop
   if (listen(server.socket, server.backlog) > 0) {
     perror("Failed to listen...\n");
     exit(1);
   }

   // Launch function
   server.launch = launch;

    return server;

}