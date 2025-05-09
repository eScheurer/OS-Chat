#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#ifndef THREADPOOL_H
#define THREADPOOL_H

#define MAX_QUEUE 128 //reicht das aus? sollte queue dynamsich wachsen?
#define INITIAL_THREADS 0
#define MAX_THREADS 128 //sinvoll?
#define THREAD_IDLE_TIMEOUT 2 // (Sekunden), sinvoll? -> später dann so 30-60 sekunden? für testing aber tief lassen
#define NEW_THREADS 4

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_t *threads;
static int thread_count;
static bool keep_running = true;
static int idle_threads = 0;

typedef struct Task {
    int socket_id;
    char task_name[64]; //for Buffer
    //TODO: how large should buffer be?
} Task;

static Task task_queue[MAX_QUEUE];
static int queue_front = 0, queue_rear =0, queue_count = 0;

int wait_for_task_with_timeout(pthread_cond_t *cond, pthread_mutex_t *lock, const int seconds);
void* thread_worker(void* arg);
void add_task_to_queue(Task task);
void shutdown_thread_pool();
void init_thread_pool();
void add_threads_to_pool();
void remove_thread_from_pool();


#endif //THREADPOOL_H
