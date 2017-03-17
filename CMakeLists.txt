cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

project(LXQServer)
add_subdirectory(src)


project(Example)
add_subdirectory(example)
