#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#include "server.h"

#include <signal.h>

#include "chatList.h"
#include "threadpool.h"

ChatList* chatList;
ChatDatabase* chatDatabase;

int main() {
    /**
    //only for testing`!!
    //extern void test_LL();
    //test_LL();
    ThreadSafeList* list = create("Chat Title");
    // Testdaten einfügen
    insert(list, "Alice: Hallo zusammen!");
    insert(list, "Bob: Hey Alice :)");
    insert(list, "Charlie: Hi alle!");
    saveToFile(list);
*/
    printf("creating ChatList \n");
    chatList = createChatList();
    chatDatabase = createChatDatabase();

    //TODO: Remove Test Code
    /**
    const char* message = "Cielle: Wow it really is working!";
    createNewChat(chatList,"general","This server has made a chat!");
    insertMessage(chatList,"general",message);
    createNewChat(chatList,"test","This server has made a chat!");
    insertMessage(chatList,"general",message);
    insertMessage(chatList,"test",message);
    char* messages = getChatMessages(chatList,"general");
    printf(messages);
    free(messages);
    printf("\n");
    char* chatNames = getChatNames(chatList);
    printf(chatNames);
    free(chatNames);
    printf("\n");
    insertMessage(chatList,"general",message);
    messages = getChatMessages(chatList,"general");
    printf(messages);
    free(messages);
    messages = getChatMessages(chatList,"test");
    printf(messages);
    free(messages);
    //End of test code
    */
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
        perror("Failed to initialize \n");
        exit(1);
    }

    // Allowing instant rebinding of socket
    int opt = 1;
    setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("Checking validity of socket, binding & listening \n");

    // Binding the socket & Check
    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind \n");
        exit(1);
    }

    // Server listens & Check
    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to listen \n");
        exit(1);
    }

    printf("-------------------------\n");
    printf("Server running! Please make sure the html is open.\n");
    printf("-------------------------\n");

    // Initialize epoll
    // See "man epoll" or https://man7.org/linux/man-pages/man7/epoll.7.html
    int epoll_instance = epoll_create1(0);
    if (epoll_instance == -1) {
        perror("Failed to create epoll \n");
        exit(EXIT_FAILURE);
    }

    // Define what to listen for
    struct epoll_event event, events[MAX_EVENTS];;
    event.events = EPOLLIN; // Waiting for incoming messages
    event.data.fd = server.socket; // Location of the event

    // Adding the server socket to the epoll listener to monitor
    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, server.socket, &event) == -1) {
        perror("Failed to add event to epoll \n");
        exit(EXIT_FAILURE);
    }

    init_thread_pool();

    //This makes it so we ignore the error if the socket we try to write to should be closed. (If someone leaves the website or refreshes it for example)
    signal(SIGPIPE, SIG_IGN);

    while (1) {
        // Waiting for event
        int n = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait failed \n");
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
                printf("New client connecting \n");
                if (client_socket == -1) {
                    perror("Failed to accept client \n");
                    continue;
                }

                // Making the newly connected client non-blocking
                setNonBlocking(client_socket);
                event.events = EPOLLIN | EPOLLET; // Notify if data is ready and edge-triggered
                event.data.fd = client_socket;

                // Adding new client to be watched by epoll
                if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("Failed to add event to epoll \n");
                    exit(EXIT_FAILURE);
                }
            } else { // Otherwise it's again data from an already connected client sending something or disconnecting early
                int client_socket = events[i].data.fd;
                char buffer[BUFFER_SIZE];
                ssize_t bytes_read;
                // Reading data from client socket
                while (true) {

                bytes_read = read(client_socket, buffer, BUFFER_SIZE-1);

                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0';

                        Task taskTest;
                        taskTest.socket_id = client_socket;
                        strcpy(taskTest.buffer, buffer);
                        add_task_to_queue(taskTest); //Pass task to threadpool to handly reading and responding
                    } else if (bytes_read == 0) {
                        printf("Client disconnected \n");
                        close(client_socket);
                        epoll_ctl(epoll_instance, EPOLL_CTL_DEL, client_socket, NULL);
                        break;
                    }else {
                        if (errno == EAGAIN && errno == EWOULDBLOCK) {
                            break;
                        } else {
                            perror("Failed to read client data \n");
                            close(client_socket);
                            epoll_ctl(epoll_instance, EPOLL_CTL_DEL, client_socket, NULL);
                            break;
                        }
                    }
                }

            }
        }
    }
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