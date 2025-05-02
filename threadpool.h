//
// Created by user1 on 14.04.25.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

typedef struct Task {
    int socket_id;
    char task_name[64]; //for Buffer
    //TODO: how large should buffer be?
} Task;

void init_thread_pool();
void add_task_to_queue(Task task);
void shutdown_thread_pool();


#endif //THREADPOOL_H
