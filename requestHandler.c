#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#include "threadpool.h"
/** this file is responsible for the cooridation of handling tasks*/

/** switch case type method that distinguishes between the differnet types of tasks.
 * @param Task that should be executded
 */

void handle_request(Task task) {
    if (strstr(task.buffer, "GET / ") != NULL) {
        extern void time_request (Task task);
        time_request(task);
        return;
    } else if (strstr(task.buffer, "GET /threadstatus ") != NULL) {
        extern void serve_thread_status(Task task);
        serve_thread_status(task);
        return;
    }

    //here:
    // else if (...)
    // continue here in a similar manner
    extern void send404(Task task);
    send404(task);
}

//here: implement your task handling in its own method