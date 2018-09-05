cmake_minimum_required(VERSION 3.1)

project(wfc)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")                                                     
include(mambaru)

add_subdirectory(config)
add_subdirectory(wfc)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

