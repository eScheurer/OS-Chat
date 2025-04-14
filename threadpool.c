//
// Created by user1 on 14.04.25.
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_QUEUE 128

static int task_queue[MAX_QUEUE];
static int queue_front = 0, queue_rear =0, queue_count = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_t *threads;
static int thread_count;
static bool keep_running = true;

/**
 * Worker thread function to handle the task (client socket)
 */
void* thread_worker(void* arg){
  while (keep_running) {
    int client_socket;

    // Wait for tasks from client socket
    pthread_mutex_lock(&lock);
    while (queue_count == 0 && keep_running) {
      pthread_cond_wait(&cond, &lock);
    }

    if (!keep_running) {
      pthread_mutex_unlock(&lock);
      break;
    }

    client_socket = task_queue[queue_front];
    queue_front = (queue_front + 1) % MAX_QUEUE;
    queue_count--;

    pthread_mutex_unlock(&lock);

    // Call send_time to handle client communication
    extern void send_time(int client_socket);
    send_time(client_socket);
  }
  return NULL;
}

void add_task_to_pool(int client_socket){
  pthread_mutex_lock(&lock);

  task_queue[queue_rear] = client_socket;
  queue_rear = (queue_rear + 1) % MAX_QUEUE;
  queue_count++;

  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&lock);
}

// Shut down threadpool
void shutdown_thread_pool(){
  keep_running = false;
  pthread_cond_broadcast(&cond);

  for (int i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }
  free(threads);
}

//Initialize threadpool
void init_thread_pool(int num_threads){
  thread_count = num_threads;
  threads = malloc(sizeof(pthread_t) * thread_count);

  //Create worker threads
  for (int i = 0; i < thread_count; i++) {
    pthread_create(&threads[i], NULL, thread_worker, NULL);
  }
}