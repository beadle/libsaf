
add_executable(pingpong_client client.cpp)
target_link_libraries(pingpong_client core pthread)

add_executable(pingpong_server server.cpp)
target_link_libraries(pingpong_server core pthread)
