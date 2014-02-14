cmake_minimum_required(VERSION 2.6)

project(wfc)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

include(Wfc)

add_subdirectory(config)
add_subdirectory(wfc)
add_subdirectory(tests)
add_subdirectory(tutorial)
