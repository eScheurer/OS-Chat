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

#include "LinkedList.h"
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
    }
    if (strstr(task.buffer, "GET /threadstatus ") != NULL) {
        extern void serve_thread_status(Task task);
        serve_thread_status(task);
        return;
    } else if (strstr(task.buffer, "GET /chatUpdate/") != NULL) {
        printf("ich bin im requestHandler richtig \n");
        char chatName[256];
        char* nameTmp = strstr(task.buffer, "chatUpdate/"); //searches for first appearance of this sting
        if (nameTmp != NULL) {
            nameTmp += strlen("chatUpdate/"); //so that it point to after the string and not before
            sscanf(nameTmp, "%[^$]", chatName); //[^$] means "read out until you get to this"
        }
        extern void sendChatUpdate(Task task, char* chatName);
        sendChatUpdate(task, chatName);
        return;
    }
    if (strstr(task.buffer, "POST /sendmessage ") != NULL) {
        extern void process_message(Task task);
        process_message(task);
        return;
    }

    //here:
    // else if (...)
    // continue here in a similar manner
    printf("ich bin im requestHandler - else case \n");

    extern void send404(Task task);
    send404(task);
}

//here: implement your task handling in its own method