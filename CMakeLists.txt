cmake_minimum_required(VERSION 3.10)
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
        threadpool.h
        chatList.h
        chatList.c
)

target_link_libraries(OS_Chat pthread rt)
# ----- Tests -----

# Executable for Unit tests
add_executable(UnitTesting
        tests/UnitTesting.c
        LinkedList.c
        threadpool.c
        chatList.c
)

# Link pthread
target_link_libraries(UnitTesting pthread)


# Executable for Integration tests
add_executable(IntegrationTesting
        tests/IntegrationTesting.c
)

# Link pthread
target_link_libraries(IntegrationTesting pthread)
