cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall")

project(book_test)

include_directories("${PROJECT_SOURCE_DIR}/src/include")
include_directories("${PROJECT_SOURCE_DIR}/../src/OrderBook.hpp")
message(${PROJECT_SOURCE_DIR})


add_executable(book_test "src/test.cpp")