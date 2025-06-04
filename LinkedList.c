#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "LinkedList.h"

// TODO Check again if no memory leaks or other funky memory stuff happens
// TODO Check that critical section is safe (Please review yourself again, mistakes can happen quickly)

/**
 * Create a new ThreadSafeList with a name
 * @param name of the list
 * @return ThreadSafeList pointer
 */
ThreadSafeList* create(const char* name) {
    ThreadSafeList* list = malloc(sizeof(ThreadSafeList));
    list->listName = strdup(name);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    pthread_mutex_init(&list->lock, NULL);
    return list;
}

/**
 * Inserting new elements into ThreadSafeList
 * @param list name of the list
 * @param name name of the user
 * @param message message of the user
 */
void insert(ThreadSafeList* list, const char* message) {
    pthread_mutex_lock(&list->lock);

    Node* new_node = malloc(sizeof(Node));
    // For strings we need to create a deep copy and therefore user strdup
    new_node->message = strdup(message);
    new_node->next = NULL;

    if (list->size == 0) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->head->next = new_node;
        list->head = new_node;
    }

    list->size++;

    if (list->size > NUM_OF_ELEMENTS_IN_LIST) {
        Node* temp = list->tail;
        list->tail = list->tail->next;

        free(temp->message);
        free(temp);

        list->size--;
    }

    pthread_mutex_unlock(&list->lock);
}

/**
 * Print out the list
 * @param list to be printed
 */
void print(ThreadSafeList* list) {
    // Locking is here required because we don't want a datarace
    pthread_mutex_lock(&list->lock);

    printf("[%s] ", list->listName);
    Node* current = list->tail;
    while (current) {
        printf("%s -> ",current->message);
        current = current->next;
    }
    printf("NULL\n");

    pthread_mutex_unlock(&list->lock);
}

// TODO Implement method to export the data of a list in json format (Warning the list in currently stored in reverse due to implementation)

char* formatMessagesForSending(const char* chatName) {
    printf("ich bin im formatMessagesForSending [%s] \n", chatName);
    const char* path = "../Chats";
    struct stat st = {0};
    // Check if ../Chats exists
    // -> If no chat exists there's also nothing to load -> skip
    if (stat(path, &st) == -1) {
        fprintf(stderr, "../Chats does not exist.\n");
        return NULL;
    }
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/Chat_%s.txt", path, chatName);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        perror("Failed to open file for reading!");
        return NULL;
    }
    size_t maxSize = 4096;
    char* messages = malloc(maxSize); //TODO: how large does it need to be?
    messages[0] = '\0'; // Initialize with empty String to avoid including previous content that was safed there
    char messageBuffer[256];

    while (fgets(messageBuffer, sizeof(messageBuffer), file)) {
        messageBuffer[strcspn(messageBuffer, "\n")] = '\0';  // removing the \n //TODO: problematic if zeilenumbruch in message?
        char temp[256]; //temp Buffer to simplify appending
        snprintf(temp, sizeof(temp), "%s $",messageBuffer);

        if (strlen(messages) + strlen(temp) + 1 > maxSize) {
            fprintf(stderr, "Return string is too long!\n");
            break;
        }
        strcat(messages, temp); // Appends the temp buffer to the end of messages-string
    }
    fclose(file);
    return messages; // Aufrufer has to free the memory of messages!
}

// TODO Implement method to send single new entries (Not sure if even possible)

/**k
 * Deallocate memory for list
 * @param list name
 */
void freeList(ThreadSafeList* list) {
    pthread_mutex_lock(&list->lock);

    Node* current = list->head;
    while (current) {
        Node* temp = current;
        current = current->next;
        free(temp->message);
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    free(list->listName);
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list);
}

/**
 * Store a list in the directory ../Chats/ with name "Chat_{nameOfList}"
 * @param list name
 */
void saveToFile(ThreadSafeList* list) {

    // Ensure if directory exists or create it
    struct stat st = {0};
    if (stat("../Chats", &st) == -1) {
        mkdir("../Chats", 0700);
    }

    pthread_mutex_lock(&list->lock);
    char filename[256];
    snprintf(filename, sizeof(filename), "../Chats/Chat_%s.txt", list->listName);

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing!");
        pthread_mutex_unlock(&list->lock);
        return;
    }

    Node* current = list->tail;
    while (current) {
        fprintf(file, "%s\n",current->message);
        current = current->next;
    }

    fclose(file);
    pthread_mutex_unlock(&list->lock);
}

/**
 * Loading a list from directory ../Chats/ with name "Chats_{name}"
 * @param name of the chat
 * @return ThreadSafeList with name
 */
ThreadSafeList* loadFromFile(const char* name) {
    const char* path = "../Chats";
    struct stat st = {0};

    // Check if ../Chats exists
    // -> If no chat exists there's also nothing to load -> skip
    if (stat(path, &st) == -1) {
        fprintf(stderr, "../Chats does not exist.\n");
        return NULL;
    }

    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/Chat_%s.txt", path, name);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        perror("Failed to open file for reading!");
        return NULL;
    }

    ThreadSafeList* list = create(name);

    // Not entirely sure why we need to reserve it first
    char nameBuffer[256];
    char messageBuffer[256];
    while (fscanf(file, "%255[^\n]",messageBuffer) == 1) {
    insert(list, messageBuffer);
}
    fclose(file);
    return list;
}