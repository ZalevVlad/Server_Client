cmake_minimum_required(VERSION 3.27)
project(Server_Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)
