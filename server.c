#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>


#include "server.h"
#include "threadpool.h"



int main() {

    // Setup for TCP connection
    printf("Server starting \n");
    struct Server server;
    server.domain = AF_INET;
    server.service = SOCK_STREAM;
    server.port = PORT;
    server.protocol = 0;
    server.backlog = 20;

    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(PORT);
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);

    server.socket = socket(AF_INET, SOCK_STREAM, 0);

    // Checking if socket was set up correctly
    if (server.socket < 0) {
        perror("Failed to initialize");
        exit(1);
    }

    // Allowing instant rebinding of socket
    int opt = 1;
    setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("Checking validity of socket, binding & listening \n");

    // Binding the socket & Check
    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind");
        exit(1);
    }

    // Server listens & Check
    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to listen");
        exit(1);
    }

    printf("Server running on http://localhost:%d\n", PORT);

    init_thread_pool(4); // Define how many threads max in threadpool

    while (1) {
        printf("Waiting for connection \n");
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        // Accepting connection
        int client_socket = accept(server.socket, (struct sockaddr*)&client_addr, &addrlen);

        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        printf("Connection accepted\n");

        add_task_to_pool(client_socket); //Pass socket to threadpool to handly reading and responding
    }
}

/**
 * Sends the time to the client
 * @param client_socket connection socket to the client
 */
void send_time(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};

    // Read the HTTP request
    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        printf("Nothing read from client.\n");
        close(client_socket);
        return;
    }

    // Print request for debugging
    printf("Received request:\n%s\n", buffer);

    // Always respond for now (simplify)
    time_t now = time(NULL);
    char *time_str = ctime(&now);

    // Basic HTML response
    char response[2048];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", strlen(time_str), time_str);

    printf("Sending response:\n%s\n", response);

    // Send response and close
    send(client_socket, response, strlen(response), 0);
    close(client_socket);
    printf("Closed client socket.\n");
}
