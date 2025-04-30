#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <pthread.h>

#define NUM_OF_ELEMENTS_IN_LIST 100

typedef struct Node {
    // TODO as time goes on we might want to store additional information such as timestamps. This requires and adaption here and also in the .c file (Ask Luis)
    char* name;
    char* message;
    struct Node* next;
} Node;

typedef struct ThreadSafeList {
    char* listName;
    Node* head;
    Node* tail;
    int size;
    pthread_mutex_t lock;
} ThreadSafeList;

ThreadSafeList* create(const char* name);
void insert(ThreadSafeList* list, const char* name, const char* message);
void print(ThreadSafeList* list);
void freeList(ThreadSafeList* list);
void saveToFile(ThreadSafeList* list);
ThreadSafeList* loadFromFile(const char* name);
void* thread_routine(void* arg);

#endif
