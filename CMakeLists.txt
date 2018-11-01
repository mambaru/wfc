cmake_minimum_required(VERSION 3.1)

project(wfc)

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  set(liblist faslib wjson wlog wflow iow wjrpc wrtstat)
  getlibs(liblist)
endif()

include_directories(.)
add_subdirectory(config)
add_subdirectory(wfc)

if ( STANDALONE AND BUILD_TESTING)
  include(FindThreads)
  find_package(Boost COMPONENTS system program_options filesystem date_time regex REQUIRED)

  set(
    wfc_liblist
    wfc
    wrtstat
    wlog
    wflow
    wjrpc
    ${Boost_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT} 
  )

  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(examples)
endif()




