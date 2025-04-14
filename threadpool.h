//
// Created by user1 on 14.04.25.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

void init_thread_pool(int num_threads);
void add_task_to_pool(int client_socket);
void shutdown_thread_pool();


#endif //THREADPOOL_H
