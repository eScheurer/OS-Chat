//
// Created by enyas on 02.05.25.
//
#include "threadpool.h"
#include <string.h>
/** this file is responsible for the cooridation of handling tasks*/

/** switch case type method that distinguishes between the differnet types of tasks.
 * @param Task that should be executded
 */

void handle_request(Task task) {
    if (strcmp(task.task_name, "TIME") == 0) {
        extern void time_request (Task task);
        time_request(task);
    }
    //here:
    // else if (...)
    // continue here in a similar manner
    // use strcmp(string1, string2) == 0 in order to compare strings in C!!!

}

//here: implement your task handling in its own method