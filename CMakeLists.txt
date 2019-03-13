cmake_minimum_required(VERSION 3.1)

project(wfc)

if ( WFC_ENABLE_STAT OR BUILD_TESTING)
  add_definitions(-DWFC_ENABLE_STAT)
endif()

include(cmake/ci.cmake)
rootlibs(faslib wjson wlog wflow iow wjrpc wrtstat)

include_directories(.)
add_subdirectory(config)
add_subdirectory(wfc)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()




