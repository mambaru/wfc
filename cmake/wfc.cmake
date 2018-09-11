if ( WFC_READY )
  return()
endif()

set(WFC_READY ON)
set(WLOG_ENABLE_CLOG ON)

if ( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
  message(FATAL_ERROR "********* FATAL: Source and build directories cannot be the same. ********* Remove CMakeCache.txt from ${CMAKE_SOURCE_DIR}")
endif()

INCLUDE(FindThreads)
set(Boost_USE_MULTITHREADED ON)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if ( WFC_BUILD_STATIC )
  message(STATUS "WFC build static")
  set(BUILD_SHARED_LIBS OFF)
  if( WFC_BUILD_STATIC_BOOST)
    message(STATUS "WFC and BOOST build static")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
  endif()
else()
  set(BUILD_SHARED_LIBS ON)
endif()

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

find_package(Boost COMPONENTS system program_options filesystem date_time REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories(wfcroot/wfc)
include_directories(wfcroot/iow)
include_directories(wfcroot/wlog)
include_directories(wfcroot/wjson)
include_directories(wfcroot/wjrpc)
include_directories(wfcroot/wflow)
include_directories(wfcroot/wrtstat)
include_directories(wfcroot/faslib)
