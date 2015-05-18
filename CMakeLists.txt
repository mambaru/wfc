cmake_minimum_required(VERSION 2.6)

project(wfc)

if ( NOT CMAKE_MODULE_PATH )
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
endif()

include(Wfc)


add_subdirectory(config)
add_subdirectory(wfc)


if ( WFC_BUILD_ALL )
  add_subdirectory(tests)
  #add_subdirectory(tutorial)
endif()

