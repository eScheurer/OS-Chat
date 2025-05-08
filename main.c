#include <stdio.h>
#include <unistd.h>  // For sleep
#include "threadpool.h"

int main() {
    // Initialize the thread pool
    init_thread_pool();
    printf("Thread pool initialized.\n");

    // Add some tasks to the queue to fill it up
    for (int i = 0; i < 4; i++) {
        Task task;
        task.socket_id = i;
        snprintf(task.task_name, sizeof(task.task_name), "Task_%d", i);
        add_task_to_queue(task);
        printf("Task %d added to the queue.\n", i);
    }

    sleep(1);

    // Test adding more tasks to see if the pool expands
    for (int i = 5; i < 10; i++) {
        Task task;
        task.socket_id = i;
        snprintf(task.task_name, sizeof(task.task_name), "Task_%d", i);
        add_task_to_queue(task);
        printf("Task %d added to the queue (9 Threads should be running -> 0-8).\n", i);
        sleep(1);
    }

    sleep(2);

    for (int i = 10; i < 20; i++) {
        Task task;
        task.socket_id = i;
        snprintf(task.task_name, sizeof(task.task_name), "Task_%d", i);
        add_task_to_queue(task);
        printf("Task %d added to the queue (More Threads).\n", i);
    }

    // Allow time for threads to process tasks
    sleep(10);

    // Check thread pool idle timeout handling
    printf("Waiting for idle timeout to see if threads terminate...\n");
    sleep(10);

    // Add one more task to see if a new thread spawns after timeout
    Task task;
    task.socket_id = 100;
    snprintf(task.task_name, sizeof(task.task_name), "Task_100");
    add_task_to_queue(task);
    printf("Added a task after idle timeout.\n");

    // Allow some time for processing
    sleep(10);

    // Shutdown the thread pool
    shutdown_thread_pool();
    printf("Thread pool shut down.\n");

    return 0;
}
