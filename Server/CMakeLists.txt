cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES "Server.cpp" "main.cpp")
set(HEADERS "Server.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})