#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "../LinkedList.h"
#include "../chatList.h"
#include "../threadpool.h"

// Forward declaration due to no headerfile
int testLinkedList();
int testChatList();
int testThreadpool();
int countMessages();



int main() {

  printf("Running LinkedList tests.\n");
  testLinkedList();
  printf("All tests for LinkedList passed.\n");

    printf("Running ChatList tests.\n");
    testChatList();
    printf("All tests for ChatList passed.\n");

    printf("Running Threadpool tests.\n");
    testThreadpool();
    printf("All tests for Threadpool passed.\n");
}

/**
 * This method tests the most important functions of the LinkedList class
 */
int testLinkedList() {

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
  assert(strcmp(messages, "TestMessage1$TestMessage2$") == 0);

  printf("getMessages() tested and passed.\n");

  freeList(list);

    return 0;

}

// Helper class for chatList tests
int countMessages(ChatList* list, const char* chatName) {
    ListNode* node = list->head;
    while (node != NULL) {
        if (strcmp(node->threadSafeList->listName, chatName) == 0) {
            return node->threadSafeList->size;
        }
        node = node->next;
    }
    return -1; // Error: Not found
}


/**
 * This method tests the most important functions of the chatList class
 */
int testChatList() {
    const char* chatName = "TestChat";
    const char* message1 = "TestMessage1";
    const char* message2  = "TestMessage2";

    ChatList* list = createChatList();
    // Test if allocation was correct
    assert(list != NULL);
    // Test if new list is empty
    assert(list->head == NULL && list->tail == NULL);


    printf("createChatList() tested and passed.\n");

    // Test if createNameChat creates new chat successfully
    createNewChat(list, chatName, message1);
    assert(list->head != NULL && list->tail != NULL);
    assert(countMessages(list, chatName) == 1);

    printf("createNewChat() tested and passed.\n");

    // Test if new Messages gets inserted properly simultaneously to test getMessages()
    insertMessage(list, chatName, message2);
    assert(list->head != NULL && list->tail != NULL);
    assert(countMessages(list, chatName) == 2);

    char* result = getChatMessages(list, chatName);
    assert(result != NULL);
    assert(strcmp(result, "TestMessage1$TestMessage2$") == 0);

    printf("insertMessage() and getChatMessages tested and passed.\n");

    free(list);

    return 0;
}



/**
 * These are methods to tests the most important functions of the threadpool class
 */
void handle_request(Task task) {
    printf("Mock handle_request: %d\n", task.socket_id);
    sleep(1);
}


int testThreadpool() {
    init_thread_pool(); // Workerthread

#define NUM_TASKS 10
    for (int i = 0; i < NUM_TASKS; i++) { // Todo: Adjust Number of tasks
        Task task;
        task.socket_id = i;
        add_task_to_queue(task);
    }

    sleep((NUM_TASKS /2) + 2); //Allow time for all tasks to complete

    shutdown_thread_pool();

    // Check how many tasks were handled
    int total_handled = 0;
    for (int i = 0; i < MAX_THREADS; i++) {
        total_handled += thread_stats[i].tasks_handled;
    }
    printf("Total handled: %d\n", total_handled);
    assert(total_handled == NUM_TASKS);
    return 0;
}
