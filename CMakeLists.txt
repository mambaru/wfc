cmake_minimum_required(VERSION 2.6)

project(comet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(.)

add_subdirectory(config)
add_subdirectory(comet)
add_subdirectory(tests)
