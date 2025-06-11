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
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;

    if (list->size > NUM_OF_ELEMENTS_IN_LIST) {
        Node* temp = list->head;
        list->head = list->head->next;

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
    Node* current = list->head;
    while (current) {
        printf("%s -> ",current->message);
        current = current->next;
    }
    printf("NULL\n");

    pthread_mutex_unlock(&list->lock);
}

char* getMessages(ThreadSafeList* list) {
    //TODO find right buffer size
    char* buffer = malloc(1000 * sizeof(char));
    pthread_mutex_lock(&list->lock);
    if (list->size == 0) {
        printf("No messages in this list: %s",list->listName);
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }

    Node* current = list->head;
    sprintf(buffer, "%s$", current->message);
    current = current->next;
    while (current) {
        sprintf(buffer+strlen(buffer), "%s$", current->message);
        current = current->next;
    }
    pthread_mutex_unlock(&list->lock);
    char* messages = malloc(strlen(buffer) + 1);
    strcpy(messages, buffer);
    free(buffer);
    return messages;
}

// Method use for old linked list:
// char* formatMessagesForSending(const char* chatName) {
//     const char* path = "../Chats";
//     struct stat st = {0};
//     // Check if ../Chats exists
//     // -> If no chat exists there's also nothing to load -> skip
//     if (stat(path, &st) == -1) {
//         fprintf(stderr, "../Chats does not exist.\n");
//         return NULL;
//     }
//     char filePath[512];
//     snprintf(filePath, sizeof(filePath), "%s/Chat_%s.txt", path, chatName);
//
//     FILE* file = fopen(filePath, "r");
//     if (!file) {
//         perror("Failed to open file for reading! error in LL");
//         return "failed to update messages..";
//     }
//     size_t maxSize = 4096;
//     char* messages = malloc(maxSize);
//     messages[0] = '\0'; // Initialize with empty String to avoid including previous content that was safed there
//     char messageBuffer[256];
//
//     while (fgets(messageBuffer, sizeof(messageBuffer), file)) {
//         messageBuffer[strcspn(messageBuffer, "\n")] = '\0';  // removing the \n
//         char temp[512]; //temp Buffer to simplify appending
//         snprintf(temp, sizeof(temp), "%s$",messageBuffer);
//
//         if (strlen(messages) + strlen(temp) + 1 > maxSize) {
//             fprintf(stderr, "Return string is too long!\n");
//             break;
//         }
//         strcat(messages, temp); // Appends the temp buffer to the end of messages-string
//     }
//     fclose(file);
//     return messages; // Aufrufer has to free the memory of messages!
// }

/**
 * Deallocate memory for list
 * @param list name
 * To ensure the list really frees all required things after the method is called ChatGPT was queried with "Is this method freeing everything it should"
 * To which ChatGPT made the important discovery that the locks needs to be discarded and only after the list freed to prevent memory leaks.
 */
void freeList(ThreadSafeList* list) {
    pthread_mutex_lock(&list->lock);

    Node* current = list->tail;
    while (current) {
        Node* temp = current;
        current = current->next;
        free(temp->message);
        free(temp);
    }
    list->tail = NULL;
    list->head = NULL;
    list->size = 0;

    free(list->listName);
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list);
}

/**
 * Store a list in the directory ../Chats/ with name "Chat_{nameOfList}"
 * @param list name
 * To get information about files https://www.geeksforgeeks.org/basics-file-handling-c/ (26.04.25) was used
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

    // Translate list into entry to file
    Node* current = list->head;
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
 * Here also https://www.geeksforgeeks.org/basics-file-handling-c/ (26.04.25) was used as guide on how to handle files)
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

    char messageBuffer[256];
    while (fscanf(file, "%255[^\n]",messageBuffer) == 1) {
    insert(list, messageBuffer);
}
    fclose(file);
    return list;
}