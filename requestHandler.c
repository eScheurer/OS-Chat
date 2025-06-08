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
#include "chatList.h"

/** this file is responsible for the cooridation of handling tasks*/

extern Database* chatDatabase;
extern Database* userDatabase;

/** switch case type method that distinguishes between the differnet types of tasks.
 * @param Task that should be executded
 */

void handle_request(Task task) {
    if (strstr(task.buffer, "GET / ") != NULL) {
        extern void time_request (Task task);
        time_request(task);
        return;
    } if (strstr(task.buffer, "GET /threadstatus ") != NULL) {
        extern void serve_thread_status(Task task);
        serve_thread_status(task);
        return;
    } if (strstr(task.buffer, "POST /chatUpdate/") != NULL) {
        extern void sendChatUpdate(Task task);
        sendChatUpdate(task);
        return;
    } if (strstr(task.buffer, "POST /sendmessage ") != NULL) {
        printf("in send msg");
        extern void process_message(Task task);
        process_message(task);
        return;
    } if (strstr(task.buffer, "POST /newChatroom/") != NULL) {
        extern void newChatroom(Task task);
        newChatroom(task);
        return;
    } if (strstr(task.buffer, "GET /chatList/") != NULL) {
        extern void allChats(Task task);
        allChats(task);
        return;
    } if (strstr(task.buffer, "GET /checkChatName/") != NULL) {
        extern void checkAndWriteName(Task task, Database* database);
        checkAndWriteName(task, chatDatabase);
        return;
    } if (strstr(task.buffer, "GET /checkUsername/") != NULL) {
        extern void checkAndWriteName(Task task, Database* database);
        checkAndWriteName(task, userDatabase);
        return;
    }
    extern void send404(Task task);
    send404(task);
}
