//
// Created by cielle on 6/4/25.
//

#ifndef CHATLIST_H
#define CHATLIST_H

#include <LinkedList.h>
#include <pthread.h>

#define NUM_OF_ELEMENTS_IN_LIST 100

typedef struct ListNode {
    ThreadSafeList* threadSafeList;
    struct ListNode* next;
} ListNode;

typedef struct ChatList {
    ListNode* head;
    pthread_mutex_t writeLock;
    pthread_mutex_t readLock;
} ChatList;

ChatList* createChatList();
void insertMessage(ChatList* chatList,const char* chatName, const char* message);
void getChat()
void printChat(ChatList* chatList,const char* chatName);
void saveChatToFile(ChatList* chatList);
void loadFromFile(ChatList* chatList, const char* chatName);
//void* thread_routine(void* arg);

#endif //CHATLIST_H
