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

ChatList* createChatList() {
    ChatList* chatList = malloc(sizeof(ChatList));
    chatList->head = NULL;
    chatList->readerCount = 0;
    pthread_mutex_init(&chatList->lock,NULL);
    pthread_mutex_init(&chatList->readLock,NULL);
    sem_init(&chatList->queue,0,1);
    return chatList;
}

void createNewChat(ChatList* chatList,const char* chatName) {
    writerLock(chatList);
    if (chatList->head == NULL) {
        //Because it's empty we actually need to edit the list, so
        //Create new node with List
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        //Add it to ChatList
        chatList->head = new_node;
        chatList->tail = new_node;

        //Now that everything is set up, we can insert the message
        insert(new_node->threadSafeList, "New Chat created!");
    } else {
        //if we haven't found it yet it is a new chat and we have to create a new node
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        chatList->tail->next = new_node;
        chatList->tail = new_node;
        insert(new_node->threadSafeList, "New Chat created!");
    }
    writerUnlock(chatList);
}

void insertMessage(ChatList* chatList, const char* chatName, const char* message) {
    readerLock(chatList);
    //Chatlist shouldn't be empty
    if (chatList->head == NULL) {
        perror("Tried inserting message but ChatList is empty!");
    } else {
        ListNode* currentNode = chatList->head;
        while (currentNode->next != NULL) {
            if (currentNode->threadSafeList->listName == chatName) {
                insert(currentNode->threadSafeList, message);
                readerUnlock(chatList);
                return;
            }
            currentNode = currentNode->next;
        }
        readerUnlock(chatList);
        printf("ERROR: Tried inserting message but couldn't find chatroom: %s",chatName);

    }
}
char* getChatMessages(ChatList* chatList, const char* chatName) {
    readerLock(chatList);
    if (chatList->head == NULL) {
        perror("Tried inserting message but ChatList is empty!");
        return NULL;
    } else {
        ListNode* currentNode = chatList->head;
        while (currentNode->next != NULL) {
            if (currentNode->threadSafeList->listName == chatName) {
                char* messages = getMessages(*currentNode->threadSafeList);
                readerUnlock(chatList);
                return messages;
            }
            currentNode = currentNode->next;
        }
        readerUnlock(chatList);
        printf("ERROR: Tried inserting message but couldn't find chatroom: %s",chatName);
        return NULL;

    }
}
//TODO: Get messages, und an locks denken!

void readerLock(ChatList* chatList) {
    sem_wait(&chatList->queue);
    pthread_mutex_lock(&chatList->readLock);
    chatList->readerCount++;
    if (chatList->readerCount == 1) {
        pthread_mutex_lock(&chatList->lock);
    }
    sem_post(&chatList->queue);
}

void readerUnlock(ChatList* chatList) {
    pthread_mutex_lock(&chatList->readLock);
    chatList->readerCount--;
    if (chatList->readerCount == 0) {
        pthread_mutex_unlock(&chatList->lock);
    }
    pthread_mutex_unlock(&chatList->readLock);
}

void writerLock(ChatList* chatList) {
    sem_wait(&chatList->queue);
    pthread_mutex_lock(&chatList->lock);
    sem_post(&chatList->queue);
}

void writerUnlock(ChatList* chatList) {
    pthread_mutex_unlock(&chatList->lock);
}