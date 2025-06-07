#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define NUM_CLIENTS 10 // Todo: test up to 100 clients
#define NUM_MESSAGES 10
#define SERVER_IP "127.0.0.1" // Todo: change for server not running on Helene machine
#define SERVER_PORT 8080

void* client_behavior(void* arg) {
  int client_id = *(int*)arg;
  free(arg);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

  // Format chat and message for this client
  for(int i = 0; i < NUM_MESSAGES; i++) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      perror("Socket creation failed\n");
      return NULL;
    }

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      perror("Connect failed\n");
      close(sock);
      return NULL;
    }

    char body[128];
    snprintf(body, sizeof(body),
         "chatroom=Room%d&message=HelloFromClient%dMsg%d",
         client_id % 10, client_id, i); // Reuse 10 rooms for stress

    char request[512];
    snprintf(request, sizeof(request),
           "POST /chat HTTP/1.1\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %ld\r\n"
           "\r\n"
           "%s",
           strlen(body),body);

     // Send request
    send(sock, request, strlen(request), 0);
    close(sock);
    usleep(1000000); // Simulate human tying
  }
  return NULL;
}

int main() {
  pthread_t threads[NUM_CLIENTS];

  for (int i = 0; i < NUM_CLIENTS; i++) {
    int* id = malloc(sizeof(int));
    *id = i;
    if (pthread_create(&threads[i], NULL, client_behavior, id) != 0) {
      perror("Failed to create thread\n");
    }
  }

  for (int i = 0; i < NUM_CLIENTS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Integration test with %d clients completed\n", NUM_CLIENTS);
  return 0;
}