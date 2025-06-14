#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdatomic.h>

#define NUM_CLIENTS 100 // Requested Number of Prof. Ciorba
#define NUM_MESSAGES 5
#define SERVER_IP "127.0.0.1" // Todo: change if not run on Helenes Lenovo Thinkpad
#define SERVER_PORT 8080

// Variable for tracking success
int failed_requests = 0;

/**
 * This method is for testing and mimics a potential behavior of a client
 *
 * ChatGPT has been used as debugging support for this test-file. (no single prompt, more of a discussion)
 * @param arg
 * @return NULL
 */
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
      atomic_fetch_add(&failed_requests, 1);
      return NULL;
    }

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      perror("Connect failed\n");
      close(sock);
      atomic_fetch_add(&failed_requests, 1);
      return NULL;
    }

    char body[128];
    snprintf(body, sizeof(body),
         "chatroom=Room%d&message=HelloFromClient%d\n",
         client_id % 10, client_id); // Reuse 10 rooms for stress, cycle through 0-9 chatroom if more than 10 clients

    char request[512];
    snprintf(request, sizeof(request),
           "POST /test HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %ld\r\n"
           "\r\n"
           "%s",
           SERVER_IP,
           strlen(body),body);

    // Send request and track
    if (send(sock, request, strlen(request), 0) < 0) {
      perror("send failed\n");
      atomic_fetch_add(&failed_requests, 1);
    }
    close(sock);
    usleep(1000000); // Simulate human tying
  }
  return NULL;
}

/**
 * Main method, used to run integrated test seen above and print out feedback
 * @return NULL
 */
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
  if (failed_requests == 0) {
    printf("All requests were handled successfully!\n");
  } else {
    printf("There were errors during the test.\n Failed requests: %d\n", failed_requests);
  }
  return 0;
}