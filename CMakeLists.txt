cmake_minimum_required(VERSION 3.31)
project(OS_Chat C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OS_Chat
        LinkedList.c
        LinkedList.h
        requestHandler.c
        server.c
        server.h
        taskHandler.c
        threadpool.c
        threadpool.h)
