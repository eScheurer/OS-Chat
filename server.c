#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 16000

// Declaring function in scope
void send_time(int client_socket);
void send_UTF_File(int client_socket,char* filename_str,char* MimeType);

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

        // Reading what was send
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            close(client_socket);
            continue;
        }

        // Listening on http://localhost:8080/
        // This can also be adapted to /response for example if the javascript sends a request to the subdirectory /response
        if (strstr(buffer, "GET / ") != NULL) {
            send_UTF_File(client_socket,"index.html","html");
            //send_UTF_File(client_socket,"script.js","javascript");
            //send_time(client_socket);
        } else {
            // Returning error if anything else
            char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(client_socket, not_found, strlen(not_found), 0);
        }
        close(client_socket);
    }

    return 0;
}


/**
 * Sends a file of type UTF-8 to the client so that the website can be viewed (like .html, .js .txt etc). Filename and MimeType are required.
 * Look up Mime types under https://www.htmlstrip.com/mime-file-type-checker for example. There is currently no errorhandling of non-existing mime-types!
 * @param client_socket connection socket to the client
 * @param filename_str Name of the file as String
 * @param MimeType Defines what type of file gets send over HTTP
 */
void send_UTF_File(int client_socket,char* filename_str,char* MimeType) {
    //Open the file
    FILE *file_ptr = fopen(filename_str, "r");
    //Check if successful
    if (file_ptr == NULL) {
        char errorMessage[50];
        snprintf(errorMessage,sizeof(errorMessage),"Failed to open requested file: %s",filename_str);
        perror(errorMessage);
    }
    //Define Header
    char response[1000];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/%s; charset=UTF-8\r\n"
             "\r\n",MimeType);
    //Read and append every line of the html file to the response
    char line[100];
    while(fgets(line, 100, file_ptr)) {
        strcat(response,line);
    }
    //Done with reading so we close the file
    fclose(file_ptr);

    printf("Sending file\n");
    send(client_socket, response, strlen(response), 0);
}

/**
 * Sends the time to the client
 * @param client_socket connection socket to the client
 */
void send_time(int client_socket) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    printf("Sending Time: %s\n", time_str);

    // Setting parameters for response
    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Content-Length: %lu\r\n"
             "\r\n"
             "%s", strlen(time_str), time_str);
    send(client_socket, response, strlen(response), 0);
}
