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

/** this file is responsible for the coordination of handling tasks*/

// Databases for unique names
extern Database* chatDatabase;
extern Database* userDatabase;

/** switch case type method that distinguishes between the different types of tasks.
 * @param task that should be executed
 */

void handle_request(Task task) {
    if (strstr(task.buffer, "GET / ") != NULL) { // Default-Option to get time, was used in prototype
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
    } if (strstr(task.buffer, "POST /checkChatName/") != NULL) {
        extern void checkAndWriteName(Task task, Database* database);
        checkAndWriteName(task, chatDatabase);
        return;
    } if (strstr(task.buffer, "POST /checkUsername/") != NULL) {
        extern void checkAndWriteName(Task task, Database* database);
        checkAndWriteName(task, userDatabase);
        return;
    } if (strstr(task.buffer, "POST /test ") != NULL) { // Used in IntegratedTesting.c
        extern void testTask(Task task);
        testTask(task);
        return;
    }
    extern void send404(Task task);
    send404(task);
}
