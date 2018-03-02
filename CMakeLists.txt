cmake_minimum_required(VERSION 2.8)

project(wfc)

include_directories(.)
include_directories(../faslib)
include_directories(../iow)
include_directories(../wjson)
include_directories(../wrtstat)

if ( NOT WFC_READY )
  link_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../iow/build/" )
endif()


add_subdirectory(config)
add_subdirectory(wfc)


if ( BUILD_TESTING )
  add_subdirectory(tests)
endif()

