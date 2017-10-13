cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

set(CMAKE_CXX_FLAGS "-g -std=c++11 ${CMAKE_CXX_FLAGS}")

project(LXQServer)
add_subdirectory(src)

project(Example)
add_subdirectory(example)
