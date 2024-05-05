cmake_minimum_required(VERSION 3.27)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(client
        main.cc
        ../message.cc
)