cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES "Client.cpp" "main.cpp")
set(HEADERS "Client.h")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} Threads::Threads)