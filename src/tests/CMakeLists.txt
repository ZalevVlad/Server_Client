cmake_minimum_required(VERSION 3.22)
project(Tests)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(tests main.cc
        ../message.cc
        message_test.h
)

# Gtest
enable_testing()
find_package(GTest REQUIRED)
target_link_libraries(tests GTest::gtest_main)

