
file(GLOB SOURCES *.cpp)

add_executable(example_simple ${SOURCES})

target_link_libraries(example_simple core pthread)
