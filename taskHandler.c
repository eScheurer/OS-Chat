#include "threadpool.h"
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

/**
* Sends the time to the client
* @param client_socket connection socket to the client
* //future use: make switch case (or similar) to distinguish between tasks, call corresponding method (which should be implemented in external file for modularisation?)
*/
void time_request(Task task) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    printf("Sending Time: %s\n", time_str);
    // Setting parameters for response
    char response[1024];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %lu\r\n"
        "\r\n"
        "%s", strlen(time_str), time_str);

    send(task.socket_id, response, strlen(response), 0);
    close(task.socket_id);
}

/**
 * Methods for Thread Monitoring
 */
void serve_thread_status(Task task) {
    char json[4096];
    get_thread_activity_json(json, sizeof(json));

    char response[8192];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: %zu\r\n\r\n%s", // Todo: only allow our ports
             strlen(json), json);

    send(task.socket_id, response, strlen(response), 0);
    close(task.socket_id);
}
