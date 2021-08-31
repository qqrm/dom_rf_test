cmake_minimum_required(VERSION 3.10)

project(test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test "src/test.cpp")

target_link_libraries(test PRIVATE Threads::Threads)