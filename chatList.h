//
// Created by cielle on 6/4/25.
//

#ifndef CHATLIST_H
#define CHATLIST_H

#include <LinkedList.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_OF_ELEMENTS_IN_LIST 100

typedef struct ListNode {
    ThreadSafeList* threadSafeList;
    struct ListNode* next;
} ListNode;

typedef struct ChatList {
    ListNode* head;
    ListNode* tail;
    int readerCount;
    pthread_mutex_t writeLock;
    pthread_mutex_t readLock;
    //Queue should be handled FIFO for fairness, semaphores usually are, but couldn't find confirmation
    sem_t queue;
} ChatList;

typedef struct DatabaseNode {
    char* name;
    struct DatabaseNode* next;
} DatabaseNode;

typedef struct ChatDatabase {
    ListNode* head;
    ListNode* tail;
} ChatDatabase;

ChatList* createChatList();
DatabaseNode* createDatabaseNode();
void createNewChat(ChatList* chatList,const char* chatName, const char* message);
void insertMessage(ChatList* chatList,const char* chatName, const char* message);
char* getChatNames(ChatList* chatList);
char* getChatMessages(ChatList* chatList, const char* chatName);
//void saveChatToFile(ChatList* chatList);
//void loadFromFile(ChatList* chatList, const char* chatName);
//void* thread_routine(void* arg);

#endif //CHATLIST_H
