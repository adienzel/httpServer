cmake_minimum_required(VERSION 3.24)
project(httpServer)

set(CMAKE_CXX_STANDARD 17)

link_libraries(
        pthread)

add_executable(httpServer main.cpp)
