cmake_minimum_required(VERSION 3.16.3)
project(parallel_bfs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(parallel_bfs main.cpp)