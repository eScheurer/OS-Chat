#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chatList.h"
#include "LinkedList.h"

//These functions should not be accessible for any other files
void readerLock(ChatList* chatList);
void readerUnlock(ChatList* chatList);
void writerLock(ChatList* chatList);
void writerUnlock(ChatList* chatList);

/**
 * Creates and returns empty chatList
 * @return empty chatList pointer
 */
ChatList* createChatList() {
    ChatList* chatList = malloc(sizeof(ChatList));
    chatList->head = NULL;
    chatList->readerCount = 0;
    pthread_mutex_init(&chatList->writeLock,NULL);
    pthread_mutex_init(&chatList->readLock,NULL);
    sem_init(&chatList->queue,0,1);
    return chatList;
}

/**
 * Creates a new Node with a LinkedList at the end of a given ChatList
 * @param chatList List in which the node will be inserted
 * @param inChatName Name of the newly created chat
 * @param inMessage 1st Message of the new chat, usually something like "UserA created new chat"
 */
void createNewChat(ChatList* chatList,const char* inChatName, const char* inMessage) {
    //To be sure we have the string before it gets de-allocated we make a hardcopy
    const char* chatName = strdup(inChatName);
    const char* message = strdup(inMessage);

    //Enter CS as writer
    writerLock(chatList);
    if (chatList->head == NULL) {
        //Create new node with List
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        //Add it to ChatList
        chatList->head = new_node;
        chatList->tail = new_node;

        //Now that everything is set up, we can insert the message
        insert(new_node->threadSafeList, message);
    } else {
        //If it's not the first Node we just append it at the end.
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        chatList->tail->next = new_node;
        chatList->tail = new_node;
        insert(new_node->threadSafeList, message);
    }
    //LeaveCS
    writerUnlock(chatList);
}

/**
 * Inserts a message into the corresponding LinkedList/Chatroom
 * @param chatList List containing the LinkedList/Chatroom
 * @param inChatName Name of the chat this message is from
 * @param inMessage Message that is to be inserted
 */
void insertMessage(ChatList* chatList, const char* inChatName, const char* inMessage) {
    //To be sure we have the string before it gets de-allocated we make a hardcopy
    const char* chatName = strdup(inChatName);
    const char* message = strdup(inMessage);

    //EnterCS as reader
    readerLock(chatList);
    //Chatlist shouldn't be empty
    if (chatList->head == NULL) {
        printf("Tried inserting message but ChatList is empty! \n");
        readerUnlock(chatList);
        return;
    }
    //If it isn't empty we iterate over the list until we find the Node
    ListNode* currentNode = chatList->head;
    while (currentNode) {
        if (strcmp(currentNode->threadSafeList->listName,chatName) == 0) {
            insert(currentNode->threadSafeList, message);
            readerUnlock(chatList);
            return;
        }
        currentNode = currentNode->next;
    }
    //If we were not able to find it we send a message into the console
    readerUnlock(chatList);
    printf("ERROR: Tried inserting message but couldn't find chatroom: %s\n",chatName);
}

/**
 * Returns a string containing all messages from a chatroom separated by a '$' separator. String needs to be freed afterwards!
 * @param chatList List containing the LinkedList/Chatroom
 * @param inChatName Name of the chat that is to be returned
 * @return String containing Messages, NULL if chat could not be found.
 */
char* getChatMessages(ChatList* chatList, const char* inChatName) {
    //To be sure we have the string before it gets de-allocated we make a hardcopy
    const char* chatName = strdup(inChatName);

    //EnterCS as reader
    readerLock(chatList);

    //Chatlist shouldn't be empty
    if (chatList->head == NULL) {
        readerUnlock(chatList);
        return NULL;
    }
    //We go over all Nodes until we find the right one
    ListNode* currentNode = chatList->head;
    while (currentNode) {
        if (strcmp(currentNode->threadSafeList->listName,chatName) == 0) {
            char* messages = getMessages(currentNode->threadSafeList);
            readerUnlock(chatList);
            return messages;
        }
        currentNode = currentNode->next;
    }

    //If we couldn't find the chat we return NULL
    //free(chatName);
    readerUnlock(chatList);
    return NULL;
}

/**
 * Returns a string containing all chat names separated by a '$' separator. String needs to be freed afterwards!
 * @param chatList List containing all chats
 * @return String containing all chat names, NULL if no chats could be found
 */
char* getChatNames(ChatList* chatList) {
    //TODO find right buffer size
    char* buffer = malloc(1000 * sizeof(char));

    //Enter CS as reader
    readerLock(chatList);
    //Chatlist shouldn't be empty
    if (chatList->head == NULL) {
        readerUnlock(chatList);
        free(buffer);
        return NULL;
    }
    //We iterate over all Nodes and add their names to the buffer
    ListNode* currentNode = chatList->head;
    //First name needs to be placed at the start of the buffer
    sprintf(buffer, "%s$", currentNode->threadSafeList->listName);
    currentNode = currentNode->next;
    while (currentNode) {
        sprintf(buffer+strlen(buffer), "%s$", currentNode->threadSafeList->listName);
        currentNode = currentNode->next;
    }
    //Leave CS
    readerUnlock(chatList);
    //Now that we are out of the CS we transfer the data into a string that is set to only the needed size
    char* chatNames = malloc(strlen(buffer) + 1);
    strcpy(chatNames, buffer);
    free(buffer);
    return chatNames;
}

/**
 * Private Function - Activates a reader lock for the chatlist
 * @param chatList
 */
void readerLock(ChatList* chatList) {
    //We wait for the semaphore to give access, this should be FIFO but it is currently (probably) not
    sem_wait(&chatList->queue);
    //If this is the first reader, activate the access-lock and block writers from entering
    pthread_mutex_lock(&chatList->readLock);
    chatList->readerCount++;
    if (chatList->readerCount == 1) {
        pthread_mutex_lock(&chatList->writeLock);
    }
    pthread_mutex_unlock(&chatList->readLock);
    //Allow next thread to try and activate a lock
    sem_post(&chatList->queue);
}

/**
 * Private Function - Deactivates a reader lock for the chatlist
 * @param chatList
 */
void readerUnlock(ChatList* chatList) {
    //We check if this is the last reader leaving the CS, if yes we unlock the access-lock
    pthread_mutex_lock(&chatList->readLock);
    chatList->readerCount--;
    if (chatList->readerCount == 0) {
        pthread_mutex_unlock(&chatList->writeLock);
    }
    pthread_mutex_unlock(&chatList->readLock);
}

/**
 * Private Function - Activates a writer lock for the chatlist
 * @param chatList
 */
void writerLock(ChatList* chatList) {
    //We wait for the semaphore to give access, this should be FIFO but it is currently (probably) not
    sem_wait(&chatList->queue);
    //We lock the CS for all other threads
    pthread_mutex_lock(&chatList->writeLock);
    //Signal to the semaphore that other threads can try and aquire the lock now
    sem_post(&chatList->queue);
}

/**
 * Private Function - Deactivates a writer lock for the chatlist
 * @param chatList
 */
void writerUnlock(ChatList* chatList) {
    //Give access to the CS to the next thread
    pthread_mutex_unlock(&chatList->writeLock);
}