cmake_minimum_required(VERSION 3.22.1)
project(OS_Chat C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OS_Chat
        server.h
        server.c
        threadpool.h
        threadpool.c
        requestHandler.c
        LinkedList.c
        taskHandler.c
)
