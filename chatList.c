#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chatList.h"
#include "LinkedList.h"

ChatList* createChatList() {
    ChatList* chatList = malloc(sizeof(ChatList));
    chatList->head = NULL;
    pthread_mutex_init(&chatList->writeLock, NULL);
    pthread_mutex_init(&chatList->readLock, NULL);
    return chatList;
}

void insertMessage(ChatList* chatList, const char* chatName, const char* message) {
    //First we disable any new readers from
    pthread_mutex_lock(&chatList->writeLock);
    pthread_mutex_lock(&chatList->readLock);
    //If Chatlist is empty we don't need to check if this chat already exists, just make a new one
    if (chatList->head == NULL) {
        //Create new node with List
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        //Add it to Chatlist
        chatList->head = new_node;

        //Now that everything is set up, we can insert the message
        insert(new_node->threadSafeList, message);
    } else {
        ListNode* currentNode = chatList->head;
        while (currentNode->next != NULL) {
            if (currentNode->threadSafeList->listName == chatName) {
                insert(currentNode->threadSafeList, message);
                pthread_mutex_unlock(&chatList->readLock);
                pthread_mutex_lock(&chatList->writeLock);
                return;
            }
            currentNode = currentNode->next;
        }

        //if we haven't found it yet it is a new chat and we have to create a new node
        ListNode* new_node = malloc(sizeof(ListNode));
        new_node->threadSafeList = create(chatName);
        new_node->next = NULL;

        currentNode->next = new_node;
        insert(new_node->threadSafeList, message);
        pthread_mutex_unlock(&chatList->readLock);
        pthread_mutex_lock(&chatList->writeLock);
    }
}

