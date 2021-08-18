cmake_minimum_required(VERSION 3.10)

project(Task)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Client)
add_subdirectory(Server)