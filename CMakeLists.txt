cmake_minimum_required(VERSION 3.1)

project(wfc)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")                                                     
include(mambaru)

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

include(cmake/ci.cmake)
get_libs(faslib wjson wlog wflow iow wjrpc wrtstat)

add_subdirectory(config)
add_subdirectory(wfc)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()

