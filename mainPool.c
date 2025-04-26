//
// Created by enyas on 23.04.25.
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#define MAX_QUEUE 128 //reicht das aus? sollte queue dynamsich wachsen?
#define INITIAL_THREADS 4
#define MAX_THREADS 128 //sinvoll?
#define THREAD_IDLE_TIMEOUT 5 // (Sekunden), sinvoll?

/** Struct for each Chunk of the queue */
typedef struct Task {
    int socket_id;
    char task_name[64]; //for Buffer
    //TODO: how large should buffer be?
} Task;

static Task task_queue[MAX_QUEUE];
static int queue_front = 0, queue_rear =0, queue_count = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_t *threads;
static int thread_count;
static bool keep_running = true;
static int active_threads = 0, idle_threads = 0;

//Funktionsprototyp
void add_thread_to_pool();
void remove_thread_from_pool();

// struct for timespec
struct timespec make_timeout_timespec(int seconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;
    return ts;
}

/**
 * @return 0 if Task available, ETIMEOUT if time out is reached
 */
int wait_for_task_with_timeout(pthread_cond_t *cond, pthread_mutex_t *lock, int seconds) {
    struct timespec ts = make_timeout_timespec(seconds);
    return pthread_cond_timedwait(cond, lock, &ts);
}

/**
 * Worker thread function to handle the task (client socket)
 */
void* thread_worker(void* arg){
    while (keep_running) {
        Task task;

        // Wait for tasks from client socket
        pthread_mutex_lock(&lock);
        idle_threads ++;

        int resume = wait_for_task_with_timeout(&cond, &lock, THREAD_IDLE_TIMEOUT);


        if (!keep_running) {
            idle_threads --;
            pthread_mutex_unlock(&lock);
            break;
        }
        if (resume == ETIMEDOUT && thread_count > INITIAL_THREADS) {
            idle_threads--;
            remove_thread_from_pool();
        }

        if (resume == 0) {
            task = task_queue[queue_front];
            queue_front = (queue_front + 1) % MAX_QUEUE;
            queue_count--;

            idle_threads --;
            active_threads ++;

            pthread_mutex_unlock(&lock);

            // Call send_time to handle client communication
            extern void handle_request(int client_socket);
            handle_request(task.socket_id);
            // hier verschiedene taskts abarbeiten

            pthread_mutex_lock(&lock);
            active_threads--;
            pthread_mutex_unlock(&lock);
        }
        else {
            pthread_mutex_unlock(&lock);
        }
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
    if ((queue_count > active_threads - idle_threads) && (thread_count < MAX_THREADS)){
        add_thread_to_pool();
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

void add_thread_to_pool() {
    pthread_t *new_threads = realloc(threads, sizeof(pthread_t) * (thread_count + 1)); //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
    if (new_threads == NULL) {
        printf("Error: Failed to allocate memory for new thread\n");
        return;
        }
        threads = new_threads;

        pthread_create(&threads[thread_count], NULL, thread_worker, NULL);
        thread_count++;
}

// Should only be called by worker threads that want to terminate themselves, not by any others, cause it will kill your thread
void remove_thread_from_pool() {
    thread_count --;
    idle_threads --;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}
