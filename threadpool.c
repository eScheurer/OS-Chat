//
// Created by enyas on 23.04.25.
//

#include "threadpool.h"

// struct for timespec
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
    while (keep_running){
        // Wait for tasks from client socket
        pthread_mutex_lock(&lock);
        Task task;
        // Idle threads automatically increased, if thread becomes available again

        idle_threads++;
        printf("Idle Threads after thread becomes available again: %d\n", idle_threads);


        const int resume = wait_for_task_with_timeout(&cond, &lock, THREAD_IDLE_TIMEOUT);

        if (!keep_running) {
            idle_threads --;
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        if (resume == ETIMEDOUT && queue_count == 0) {
            pthread_mutex_unlock(&lock);
            remove_thread_from_pool();
            return NULL;
        }

        if (resume == 0 || (resume == ETIMEDOUT && queue_count > 0)) {
            task = task_queue[queue_front];
            queue_front = (queue_front + 1) % MAX_QUEUE;
            queue_count--;
            idle_threads --;
            pthread_mutex_unlock(&lock);

            extern void handle_request(Task task); // Call send_time to handle client communication
            handle_request(task);

        } else {
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

    task_queue[queue_rear] = task;
    queue_rear = (queue_rear + 1) % MAX_QUEUE;
    queue_count++;

// first add task to queue and then create new thread to ensure right order and safe storage of task (and no race condition)

    if (queue_count > idle_threads && thread_count < MAX_THREADS){
        printf("Adding new threads.\n");
        pthread_mutex_unlock(&lock);
        add_threads_to_pool();
        pthread_mutex_lock(&lock);
    }

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
    pthread_mutex_lock(&lock);
    thread_count = INITIAL_THREADS;
    threads = malloc(sizeof(pthread_t) * thread_count);
    if (threads == NULL) {
        perror("Error: Failed to allocate memory for thread pool");
        pthread_mutex_unlock(&lock);
        return;
    }

    // Create worker threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, thread_worker, NULL);
    }

    pthread_mutex_unlock(&lock);
}

void add_threads_to_pool() {
    pthread_mutex_lock(&lock);
    pthread_t *new_threads = realloc(threads, sizeof(pthread_t) * (thread_count + 1)); //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
    if (new_threads == NULL) {
        perror("Error: Failed to allocate memory for new thread");
        pthread_mutex_unlock(&lock);
        return;
    }
    threads = new_threads;

    if (pthread_create(&threads[thread_count], NULL, thread_worker, NULL) != 0) {
        perror("Error: Failed to create new thread");
        pthread_mutex_unlock(&lock);
        return;
    }
    thread_count++;
    printf("Creating new Thread %d .\n", thread_count); //for testing
    pthread_mutex_unlock(&lock);
}

/** idea for adding multiple threads (auskommentiert), doesn't fully work yet (sometimes works, sometimes creates complete overhead..)
 * void add_threads_to_pool() {
    pthread_mutex_lock(&lock);
    pthread_t *new_threads = realloc(threads, sizeof(pthread_t) * (thread_count + NEW_THREADS)); //first realloc to new place instead of overwriting to avoid losing memory leak in case of error
    if (new_threads == NULL) {
        printf("Error: Failed to allocate memory for new thread\n");
        return;
        }

    for (int i = NEW_THREADS -1; i >= 0; i-=1) {
        thread_count++;
        pthread_create(&new_threads[thread_count -1], NULL, thread_worker, NULL);
        printf("Creating new Thread %d .\n", thread_count); //for testing
    }
    threads = new_threads;
    pthread_mutex_unlock(&lock);
}
*/

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
                idle_threads--;
                printf("Idle Threads after shutting some down: %d\n", idle_threads);
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        printf("Thread %d is exiting due to inactivity.\n", thread_count); //for testing
        pthread_exit(NULL);
    }
    printf("Persistent %d threads not removed\n", INITIAL_THREADS);
    //idle_threads++;
    pthread_mutex_unlock(&lock);
}

