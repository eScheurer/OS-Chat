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
    } else if (strstr(task.buffer, "GET /chatUpdate/name:") != NULL) { //idk how this works yet and what I need to put here..
         //aus Buffer extrahieren; char* chatName =
        char chatName[256];
        strcpy(chatName, "TestChat");
        extern void sendChatUpdate(Task task, char* chatName);
        sendChatUpdate(task, chatName);
    }

    //here:
    // else if (...)
    // continue here in a similar manner
    extern void send404(Task task);
    send404(task);
    else {
        char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(task.socket_id, not_found, strlen(not_found), 0);
    }
}

//here: implement your task handling in its own method