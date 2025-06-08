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

#include "chatList.h"
#include "server.h"

char* extractHTTPBody(Task task);
void send404(Task task);

extern ChatList* chatList;
extern Database* chatDatabase;
extern Database* userDatabase;
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
}

/**
 * Reads out chatroom as well as message-content of a sent message. Stores it in the corresponding linked list.
 * @param task containing http buffer
 */
void process_message(Task task) {
    printf("received message, processing... \n");
    //Get Message Body - Dynamically allocated!
    char* body = extractHTTPBody(task);
    if (body == NULL) {
        free(body);
        return;
    }

    //Get Chat Name
    const long chatNameLen = strstr(body,":") - body;
    char chatName[chatNameLen+1];
    chatName[chatNameLen] = '\0';
    for (int i = 0; i < chatNameLen; i++) {
        chatName[i] = *(body+i);
    }

    //Get Actual Chat Message
    const char* message = body+chatNameLen+1;

    //We insert strings into list, which will get copied
    insertMessage(chatList, chatName, message);
    //After we're done free the body of the dynamically allocated body & all used strings here again.
    free(body);
}

/**
 *  Reads out the current content of a chat and sends it to client.
 *  @param task contains the name of the current chatroom
 */
void sendChatUpdate(Task task) {
    char* chatName = extractHTTPBody(task);
    if (chatName == NULL) {
        send404(task);
        free(chatName);
        return;
    }
    //char chatName2[512] = "Chat Title";
    char* messages = getChatMessages(chatList, chatName);
    if (messages == NULL) {
        printf("Failed to send Chat update, chat could not be found\n");
        free(messages);
        //messages = "Failed to load, trying again...";
        return;
    };
    char response[1024];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %lu\r\n"
        "\r\n"
        "%s", strlen(messages), messages);
    send(task.socket_id, response, strlen(response), 0);
}

/**
 * Sends client a 404 response
 * @param task containing socket_id
 */
void send404(Task task) {
    printf("404: invalid request made\n");
    /**
    char* httpRequest = strdup(task.buffer);
    char test[100];
    printf("Buffer content: \n");
    snprintf(test, sizeof(test), httpRequest);
    printf(test);
    printf("\n");
*/

    const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(task.socket_id, not_found, strlen(not_found), 0);
}

/**
 * Adds initial message to the chatroom in creation
 * @param task contains the buffer with chat name and msg
 */
void newChatroom(Task task) {
    char* body = extractHTTPBody(task);
    if (body == NULL) {
        send404(task);
        free(body);
        return;
    }
    //Get Chat Name
    const long chatNameLen = strstr(body,":") - body;
    char chatName[chatNameLen+1];
    chatName[chatNameLen] = '\0';
    for (int i = 0; i < chatNameLen; i++) {
        chatName[i] = *(body+i);
    }
    //Get Actual Chat Message
    const char* message = body+chatNameLen+1;
    //We insert strings into list, which will get copied
    createNewChat(chatList,chatName,message);
    //After we're done free the body of the dynamically allocated body & all used strings here again.
    free(body);
}


/** sends list of currently active chatrooms to client
 * @param task
 */
void allChats(Task task) {
    char* message = getChatNames(chatList);
    char response[1024];
    snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Length: %lu\r\n"
            "\r\n"
            "%s", strlen(message), message);
    send(task.socket_id, response, strlen(response), 0);
}


/**
 * Extracts the Body of a http message.
 * @param task containing http message in buffer.
 * @return body, string that needs to be deallocated!!
 */
char* extractHTTPBody(Task task) {
    //Search for Body length in HTTP Header
    char* ptr =  strstr(task.buffer, "Content-Length:");
    if (ptr == NULL) {
        printf("Error: Could not read received message from buffer\n");
        return NULL;
    }

    //We need to skip past the "Content-Length: " part of this line, so we add 16 and then convert the string to an integer
    const int bodyLength = strtol(ptr+16,NULL,10);
    //printf("Received body length: %d\n", bodyLength);

    //Create new char array to store extracted buffer
    char* body = (char*)malloc((bodyLength+1)*sizeof(char));
    if (body == NULL) {
        printf("Memory allocation failed!\n");
        free(body);
        return NULL;
    }
    body[bodyLength] = '\0';
    int headerLength = (int) strlen(task.buffer) - bodyLength;
    for (int i = 0 ; i < bodyLength; i++) {
        body[i] = task.buffer[i+headerLength];
    }
    //printf("Received body: %s\n", body);
    return body;
}