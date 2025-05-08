//
// Created by enyas on 02.05.25.
//
#include <stdio.h>

#include "threadpool.h"
#include <unistd.h>
#include <string.h>


void handle_request(Task task) {
    printf("Handling task from socket %d with name: %s\n", task.socket_id, task.task_name);
    sleep(5);  // Simulate task processing
}

