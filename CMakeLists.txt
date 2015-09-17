cmake_minimum_required(VERSION 2.6)

project(wfc)

include_directories(../faslib)
include_directories(../iow/)

if ( NOT WFC_READY )
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  include(Wfc)
endif()

add_subdirectory(config)
add_subdirectory(wfc)


if ( WFC_BUILD_ALL )
  add_subdirectory(tests)
endif()

