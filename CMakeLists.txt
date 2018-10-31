cmake_minimum_required(VERSION 3.1)

project(wfc)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(mambaru)

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

include(ci)
if ( STANDALONE )
  set(liblist faslib wjson wlog wflow iow wjrpc wrtstat)
  getlibs(liblist)
endif()

add_subdirectory(config)
add_subdirectory(wfc)

if ( STANDALONE )
  if ( BUILD_TESTING )
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(examples)
  endif()
endif()




