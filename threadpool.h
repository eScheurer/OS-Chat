//
// Created by user1 on 14.04.25.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>


#define MAX_THREADS 128 // Todo: does this make sense?

// Struct for each Chunk of the queue
typedef struct Task {
    int socket_id;
    char buffer[16000]; //for Buffer
    //TODO: how large should buffer be?
} Task;

/**
 * Methods for dynamic Threadpool
 */
void init_thread_pool();
void add_task_to_queue(Task task);
void shutdown_thread_pool();

// Struct for Thread Statistics
typedef struct ThreadStats { // typedef to define type later
    pthread_t thread_id;
    int tasks_handled;
    double total_active_time;
    bool is_idle; // id_idel = true when thread is waiting, is_idle = false when thread busy
} ThreadStats; //

extern ThreadStats thread_stats[MAX_THREADS];
extern int thread_count;

/**
 * Methods for Monitoring Threads and save Statistics
 */
void get_thread_activity_json(char *buffer, size_t buffer_size);


#endif //THREADPOOL_H
