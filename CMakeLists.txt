cmake_minimum_required(VERSION 3.1)

project(wfc)

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  getlibs(faslib wjson wlog wflow iow wjrpc wrtstat)
endif()

include_directories(.)
add_subdirectory(config)
add_subdirectory(wfc)

if ( BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()




