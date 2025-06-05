cmake_minimum_required(VERSION 3.2)
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

# ----- Tests -----

# Executable for testfiles
add_executable(UnitTesting
        tests/UnitTesting.c
        LinkedList.h
        LinkedList.c
        threadpool.h
        threadpool.c
)

# Link pthread
target_link_libraries(UnitTesting pthread)
