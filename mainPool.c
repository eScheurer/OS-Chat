//
// Created by enyas on 23.04.25.
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_QUEUE 128 //reicht das aus? sollte queue dynamsich wachsen?
#define INITIAL_THREADS 4

/** Struct for each Chunk of the queue */
typedef struct Task {
    int socket_id;
    char task_name[64]; //for Buffer
    //TODO: how large should buffer be?
} Task;

//typedef struct Task_queue {
  //  Task task; //info what to do
    //int count; // ?????
    //int read_pos; //pointer to where to read out the next task
//} Task_queue;

static Task task_queue[MAX_QUEUE];
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
        Task task;

        // Wait for tasks from client socket
        pthread_mutex_lock(&lock);
        while (queue_count == 0 && keep_running) {
            pthread_cond_wait(&cond, &lock);
        }

        if (!keep_running) {
            pthread_mutex_unlock(&lock);
            break;
        }

        task = task_queue[queue_front];
        queue_front = (queue_front + 1) % MAX_QUEUE;
        queue_count--;

        pthread_mutex_unlock(&lock);

        // Call send_time to handle client communication
        extern void handle_request(int client_socket);
        handle_request(task.socket_id);
        // hier verschiedene taskts abarbeiten
    }
    return NULL;
}

void add_task_to_queue(Task task){
    pthread_mutex_lock(&lock);

    //error handling
    if (queue_count == MAX_QUEUE) {
        printf("Error: queue is full\n");
        pthread_mutex_unlock(&lock);
        return;
    }

    task_queue[queue_rear] = task;
    queue_rear = (queue_rear + 1) % MAX_QUEUE;
    queue_count++;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

// Shut down threadpool
void shutdown_thread_pool(){
    pthread_mutex_lock(&lock);
    keep_running = false;
    pthread_cond_broadcast(&cond); //wake up all existing threads
    pthread_mutex_unlock(&lock);

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}

//Initialize threadpool
void init_thread_pool(){
    thread_count = INITIAL_THREADS;
    threads = malloc(sizeof(pthread_t) * thread_count);

    //Create worker threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, thread_worker, NULL);
    }
}
