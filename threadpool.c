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
#include "threadpool.h"
#include <string.h>

#define MAX_QUEUE 512 // Tested with up to 200 clients connecting
#define INITIAL_THREADS 4
#define THREAD_IDLE_TIMEOUT 8 // (Sekunden)
#define NEW_THREADS 4

static Task task_queue[MAX_QUEUE];
static int queue_front = 0, queue_rear =0, queue_count = 0;

ThreadStats thread_stats[MAX_THREADS]; // Initialize when creating thread

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_t *threads;
int thread_count = 0;
static bool keep_running = true;
static int idle_threads = 0;

// Funktionsprototyp
void add_threads_to_pool();
void remove_thread_from_pool();
void print_threadpool_status();

// Struct for timespec
struct timespec make_timeout_timespec(const int seconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;
    return ts;
}

/**
 * @return 0 if Task available, ETIMEOUT if time out is reached
 */

int wait_for_task_with_timeout(pthread_cond_t *cond, pthread_mutex_t *lock, const int seconds) {
    const struct timespec ts = make_timeout_timespec(seconds);
    return pthread_cond_timedwait(cond, lock, &ts);
}

/**
 * Worker thread function to handle the task (client socket)
 */
void* thread_worker(void* arg) {
    // printf("Thread %ld handling request\n", pthread_self()); // Used for IntegartionTesting

    int index = *((int*)arg); // Pass thread index when creating
    free(arg);

    pthread_mutex_lock(&lock);
    thread_stats[index].thread_id = pthread_self();
    thread_stats[index].tasks_handled = 0;
    thread_stats[index].total_active_time = 0;
    thread_stats[index].is_idle = true;
    pthread_mutex_unlock(&lock);

    while (keep_running){
        // Wait for tasks from client socket
        pthread_mutex_lock(&lock);
        Task task;
        idle_threads ++; //markiert sich selbst als idle um zu zeigen dass er auf arbeit wartet


        // thread geht schlafen
        //const int resume = wait_for_task_with_timeout(&cond, &lock, THREAD_IDLE_TIMEOUT);
        //wird geweckt, wenn jemand pthread_cond_signal(&cond) oder pthread_cond_broadcast(&cond) aufruft.
        //Oder er wird nach THREAD_IDLE_TIMEOUT Sekunden automatisch aufgeweckt, falls niemand ihn vorher weckt.


        int resume;
        //wartet auf Task oder Timeout, geht nur raus wenn eine Task da oder thread beendet werden soll
        while (queue_count == 0 && keep_running) {
            resume = wait_for_task_with_timeout(&cond, &lock, THREAD_IDLE_TIMEOUT);

            if (!keep_running) {
                idle_threads--;
                pthread_mutex_unlock(&lock);
                return NULL;
            }
            if (resume == ETIMEDOUT && queue_count == 0) {
                idle_threads--;
                pthread_mutex_unlock(&lock);
                remove_thread_from_pool();
                return NULL;
            }
        }

        //if (resume == 0 && queue_count > 0 || (resume == ETIMEDOUT && queue_count > 0))
            // (resume == 0, wenn thread durch siganl geweckt wurde
        task = task_queue[queue_front]; //task wird aus der queue geholt
        queue_front = (queue_front + 1) % MAX_QUEUE;
        queue_count--;
        idle_threads --;
        pthread_mutex_unlock(&lock);

        // Monitoring
        thread_stats[index].is_idle = false;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        extern void handle_request(Task task); // Call send_time to handle client communication
        handle_request(task);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec ) / 1e9;
        thread_stats[index].total_active_time += elapsed;
        thread_stats[index].tasks_handled++;
        thread_stats[index].is_idle = true;
        close(task.socket_id);
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

    // first add task to queue and then create new thread to ensure right order and safe storage of task (and no race condition)

    if (queue_count > idle_threads && thread_count < MAX_THREADS){
        pthread_mutex_unlock(&lock);
        add_threads_to_pool();
        pthread_mutex_lock(&lock);
    }

    pthread_cond_signal(&cond); //thread wird aufgeweckt!
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
    pthread_mutex_lock(&lock);
    thread_count = INITIAL_THREADS;
    threads = malloc(sizeof(pthread_t) * thread_count);

    //Create worker threads
    for (int i = 0; i < thread_count; i++) {
        int* arg = malloc(sizeof(int));
        *arg= i;
        pthread_create(&threads[i], NULL, thread_worker, arg);
    }
    pthread_mutex_unlock(&lock);
}

void add_threads_to_pool() {
    pthread_mutex_lock(&lock);
    //reallocate memory: with pointer to the memory that is being resized + new size of allocated memory
    pthread_t *new_threads = realloc(threads, sizeof(pthread_t) * (thread_count + 1));
    if (new_threads == NULL) {
        printf("Error: Failed to allocate memory for new thread\n");
        pthread_mutex_unlock(&lock);
        return;
    }

    //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
    threads = new_threads;
    thread_count++;
    int* arg = malloc(sizeof(int));
    *arg = thread_count -1;
    pthread_create(&threads[thread_count -1], NULL, thread_worker, arg);
    printf("Creating new Thread %d.\n", thread_count); //for testing
    pthread_mutex_unlock(&lock);
}

// //idea for adding multiple threads (auskommentiert), doesn't fully work yet (sometimes works, sometimes creates complete overhead..)
// void add_threads_to_pool() {
//     pthread_mutex_lock(&lock);
//
//     const int oldCount = thread_count;
//
//     //reallocate memory: with pointer to the memory that is being resized + new size of allocated memory
//     pthread_t *new_threads = realloc(threads, sizeof(pthread_t) * (thread_count + NEW_THREADS)); //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
//     if (new_threads == NULL) {
//         printf("Error: Failed to allocate memory for new thread\n");
//         pthread_mutex_unlock(&lock);
//         return;
//     }
//     //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
//     threads = new_threads;
//
//     for (int i = 0; i < NEW_THREADS; i++) {
//         const int index = oldCount + i;
//         if (pthread_create(&new_threads[index], NULL, thread_worker, NULL) != 0) {
//             printf("Error: Failed to create thread %d\n", oldCount + index);
//         } else {
//             thread_count++; //only increase if really sucessful
//             printf("Creating new Thread %d .\n", thread_count); //for testing
//         }
//     }
//     pthread_mutex_unlock(&lock);
// }

// Should only be called by worker threads that want to terminate themselves, not by any others, cause it will kill your thread
void remove_thread_from_pool() {
    pthread_mutex_lock(&lock);

    if (thread_count > INITIAL_THREADS) {
        for (int i = 0; i < thread_count; i++) {
            if (pthread_equal(threads[i], pthread_self())) { //moves all existing threads to the left to avoid gaps in array
                for (int j = i; j < thread_count - 1; j++) {
                    threads[j] = threads[j + 1];
                }
                thread_count--;
                break;
            }
        }

        pthread_mutex_unlock(&lock);
        printf("Thread %d is exiting due to inactivity.\n", thread_count); //for testing
        pthread_exit(NULL);
    }
    printf("Essential Thread: not gonna remove youuu\n");
    pthread_mutex_unlock(&lock);
}


/**
 * Methods for Thread Monitoring
 * Extension to display one focus of this OS-project: threadpooling
 */

void get_thread_activity_json(char *buffer, size_t buffer_size) {
    pthread_mutex_lock(&lock);

    snprintf(buffer, buffer_size, "[");
    size_t used = 1;

    for (int i = 0; i < thread_count; ++i) {
        if (thread_stats[i].thread_id == 0) {
            continue; // skip unitialized thread
        }
        int written = snprintf(buffer + used, buffer_size - used,
            "{\"thread_id\": %lu, \"tasks_handled\": %d, \"active_time\": %.2f, \"is_idle\": %s}%s",
            (unsigned long)thread_stats[i].thread_id,
            thread_stats[i].tasks_handled,
            thread_stats[i].total_active_time,
            thread_stats[i].is_idle ? "true" : "false",
            (i < thread_count - 1 && thread_stats[i + 1].thread_id != 0) ? "," : "");
        used += written;
        if (used >= buffer_size) break;
    }
    snprintf(buffer + used, buffer_size - used, "]");
    pthread_mutex_unlock(&lock);
}

