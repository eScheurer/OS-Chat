cmake_minimum_required(VERSION 3.31)
project(OS_Chat C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OS_Chat
        LinkedList.c
        LinkedList.h
        requestHandler.c
        server.c
        LinkedListStub.c
        server.h
        taskHandler.c
        threadpool.c
        threadpool.h
        chatList.h
        chatList.c
)

target_link_libraries(OS_Chat pthread rt)