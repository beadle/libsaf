cmake_minimum_required(VERSION 3.0)

project(libsaf)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()


# config cxx flags
set(CXX_FLAGS
-g
-std=c++11
-Wall
# -Wextra
# -Werror
# -Wconversion
# -Wno-unused-parameter
# -Wold-style-cast
# -Woverloaded-virtual
# -Wpointer-arith
# -Wshadow
# -Wwrite-strings
# -march=native
# -rdynamic
)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")


# config build type
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")


# config output paths
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/lib)


add_subdirectory(src)

add_subdirectory(example)
