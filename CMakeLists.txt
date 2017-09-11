cmake_minimum_required(VERSION 2.6)

project(wfc)

include_directories(.)
include_directories(../faslib)
include_directories(../iow)
include_directories(../wjson)
include_directories(../wrtstat)

add_subdirectory(config)
add_subdirectory(wfc)


if ( BUILD_TESTING )
  #add_subdirectory(tests)
endif()

