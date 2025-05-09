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
    if (strcmp(task.task_name, "GET /") == 0) {
        extern void time_request (Task task);
        time_request(task);
    }
    if (strcmp(task.task_name, "TIME") == 0) {
        extern void time_request (Task task);
        time_request(task);
    }

    //here:
    // else if (...)
    // continue here in a similar manner
    // use strcmp(string1, string2) == 0 in order to compare strings in C!!!

    char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(task.socket_id, not_found, strlen(not_found), 0);

}

//here: implement your task handling in its own method