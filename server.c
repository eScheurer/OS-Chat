#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "server.h"
#include "chatList.h"
#include "threadpool.h"

ChatList* chatList;
Database* chatDatabase;
Database *userDatabase;


/**
 * Helperfunction to receive full request before proceed in requestHandler
 *
 * ChatGPT has been used to scetch this method and debug it later
 * initial prompt: how can i fix my program to read full incoming HTTp request before checking with strstr(...)
 * error prompt: zsh: segmentation fault (core dumped)  ./build/OS_Chat
*/
ssize_t recv_full_request(int socket, char *buffer, size_t buffer_size) {
    ssize_t total_received = 0;
    while (1) {
        ssize_t bytes_received = recv(socket, buffer + total_received, buffer_size - total_received -1, 0);
        if (bytes_received <= 0) {
            return -1; // Error or closed connection
        }

        total_received += bytes_received;
        buffer[total_received] = '\0';

        // Check if full request has been received
        if (strstr (buffer, "\r\n\r\n") != NULL) {
            break; // End found
        }
    }
    return total_received;
}

/**
 * Gets the file status of a file and setts the O_NONBLOCK flag.
 * This ensures that the socket isn't blocking. (Not waiting for accept, read or write like in the prototype before)
 * @param fd Socket to make non blocking
 * @return Exit status of operation (Unused)
 */
int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


/**
 * Main function for Server
 * @return
 */
int main() {
    chatList = createChatList();
    printf("ChatList created.\n");
    chatDatabase = createDatabase();
    userDatabase = createDatabase();

    // Setup for TCP connection
    printf("Server starting... \n");
    struct Server server;
    server.domain = AF_INET;
    server.service = SOCK_STREAM;
    server.port = PORT;
    server.protocol = 0;
    server.backlog = 128;

    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(PORT);
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);

    server.socket = socket(AF_INET, SOCK_STREAM, 0);

    // Checking if socket was set up correctly
    if (server.socket < 0) {
        perror("Failed to initialize. \n");
        exit(1);
    }

    // Allowing instant rebinding of socket
    int opt = 1;
    setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("Checking validity of socket, binding & listening. \n");

    // Binding the socket & Check
    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind. \n");
        exit(1);
    }

    // Server listens & Check
    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to listen. \n");
        exit(1);
    }

    // Everything running fine so far
    printf("-------------------------\n");
    printf("Server running! Please make sure the website is running.\n");
    printf("-------------------------\n");

    // Initialize epoll
    // See "man epoll" or https://man7.org/linux/man-pages/man7/epoll.7.html
    // The below code is inspired by an example featured on the man page but also modified to fit our needs with new clients connecting and already existing clients state keeping
    int epoll_instance = epoll_create1(0);
    if (epoll_instance == -1) {
        perror("Failed to create epoll. \n");
        exit(EXIT_FAILURE);
    }

    // Define what to listen for
    struct epoll_event event, events[MAX_EVENTS];;
    event.events = EPOLLIN; // Waiting for incoming messages
    event.data.fd = server.socket; // Location of the event

    // Adding the server socket to the epoll listener to monitor
    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, server.socket, &event) == -1) {
        perror("Failed to add event to epoll. \n");
        exit(EXIT_FAILURE);
    }

    // Populating thread pool
    init_thread_pool();

    // This makes it so we ignore the error if the socket we try to write to should be closed. (If someone leaves the website or refreshes it for example)
    // This change was suggested by ChatGPT after trying to debug a crash that happened after someone closed the tab.
    // The query used was "When a client closes their tab the whole server crashes. Please find and suggest a fix."
    signal(SIGPIPE, SIG_IGN);

    while (1) {
        // Waiting for event
        int n = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait failed. \n");
            exit(EXIT_FAILURE);
        }
        // Looping through all file descriptors (sockets)
        for (int i = 0; i < n; i++) {
            // If the event is triggered by the server socket, then a new client connected
            // Accepting new connection from client -> Arrive on server socket
            if (events[i].data.fd == server.socket) {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                // Accepting the connection from the connecting client
                int client_socket = accept(server.socket, (struct sockaddr *)&client, &len);
                //printf("New payload received. \n");
                if (client_socket == -1) {
                    perror("Failed to accept client. \n");
                    continue;
                }

                // Making the newly connected client non-blocking
                setNonBlocking(client_socket);
                event.events = EPOLLIN | EPOLLET; // Notify if data is ready and edge-triggered
                event.data.fd = client_socket;

                // Adding new client to be watched by epoll
                if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("Failed to add event to epoll. \n");
                    exit(EXIT_FAILURE);
                }
            } else { // Otherwise it's again data from an already connected client sending something or disconnecting early
                int client_socket = events[i].data.fd;
                char buffer[BUFFER_SIZE];
                // Reading full request from client socket
                ssize_t bytes_read = recv_full_request(client_socket, buffer, BUFFER_SIZE);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    // printf("Received full request: \n%s\n", buffer); // For debugging and testing
                    // Create the task to pass on
                    Task taskTest;
                    taskTest.socket_id = client_socket;
                    strcpy(taskTest.buffer, buffer);
                    add_task_to_queue(taskTest); //Pass task to threadpool to handle reading and responding

                    // no bytes ready means a client disconnected
                } else if (bytes_read == 0) {
                    printf("Client disconnected. \n");
                    close(client_socket);
                    epoll_ctl(epoll_instance, EPOLL_CTL_DEL, client_socket, NULL);

                    // If the bytes read were <0 we have another issue
                } else {
                    if (errno == EAGAIN && errno == EWOULDBLOCK) {
                            break;
                    } else {
                            perror("Failed to read client data. \n");
                            close(client_socket);
                            epoll_ctl(epoll_instance, EPOLL_CTL_DEL, client_socket, NULL);
                    }
                }
            }
        }
    }
}

