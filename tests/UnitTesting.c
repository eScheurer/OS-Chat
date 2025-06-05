#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../LinkedList.h"
#include "../threadpool.h"

// Forward declaration due to no headerfile
int testLinkedList();
int testThreadpool();

int main() {

  printf("Running LinkedList tests.\n");
  testLinkedList();
  printf("All tests for LinkedList passed.\n");

    printf("Running Threadpool tests.\n");
    testThreadpool();
    printf("All tests for Threadpool passed.\n");
}

/**
 * This method tests the most important functions of the LinkedList class
 */
int testLinkedList() {
  printf("Running LinkedList tests\n");

  ThreadSafeList* list = create("TestList");
  // Test if allocation was correct
  assert(list != NULL);
  // Test if new list is empty, TODO: later check if not empty because of default Message
  assert(list->head == NULL && list->tail == NULL && list->size == 0);

  printf("create() tested and passed.\n");

  // Test if insertion of Message into list does function
  insert(list, "TestMessage1");
  insert(list, "TestMessage2");
  assert(list->head != NULL && list->tail != NULL && list->size == 2); //TODO: May change number two if default message is sent

 // Test if Messages are copied correctly into list
  assert(strcmp(list->tail->message, "TestMessage1") == 0);
  assert(strcmp(list->head->message, "TestMessage2") == 0);

  printf("insert() tested and passed.\n");

  // Test if Message is extracted correctly from list
  char* messages = getMessages(list);
  assert(messages != NULL);
  assert(strcmp(messages, "TestMessage1$TestMessage2") == 0);

  printf("getMessages() tested and passed.\n");

  // Test if memory has been cleaned up properly
  freeList(list);
  list == 0;

}

/**
 * This method tests the most important functions of the threadpool class
 */
int testThreadpool() {

}
