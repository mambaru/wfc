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

IF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")
    execute_process(
      ${CMAKE_CXX_COMPILER} -dumpversion
      OUTPUT_VARIABLE gcc_compiler_version
    )

    STRING(REGEX REPLACE "^([0-9]+)\\..*" "\\1" gcc_major_version "${gcc_compiler_version}")
    STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" gcc_minor_version "${gcc_compiler_version}")
    MATH(EXPR gcc_version_number "${gcc_major_version} * 1000 + ${gcc_minor_version}" )

    message(STATUS "C++ compiler version: ${gcc_compiler_version} major: ${gcc_major_version} minor: ${gcc_minor_version} number: ${gcc_version_number} [${CMAKE_CXX_COMPILER}]")

    SET(WFC_CXX_STANDARD    "-std=c++11")
    SET(WFC_CXX_WARN_FLAGS  "-W -Wall -pedantic -Wextra")
    if ( NOT ${gcc_version_number} LESS 4008 )
      SET(WFC_CXX_WARN_FLAGS    "${WFC_CXX_WARN_FLAGS} -ftemplate-backtrace-limit=0 ")
    endif()

    SET(WFC_CXX_DEFINES     "-D_THREAD_SAFE -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DBOOST_SYSTEM_NO_DEPRECATED -D_GLIBCXX_USE_NANOSLEEP")
    SET(WFC_CXX_ARCH_FLAGS  "-mfpmath=sse -msse2")
    SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${WFC_CXX_STANDARD} ${WFC_CXX_WARN_FLAGS} ${WFC_CXX_DEFINES} ${WFC_CXX_ARCH_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ") 
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ") 
    SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb ")
    SET(CMAKE_C_FLAGS_RELEASE "-O2 -ggdb -march=core2 -DNDEBUG")
ENDIF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")

set(FASLIB_DIR "${CMAKE_SOURCE_DIR}/wfcroot/faslib")
set(IOWLIB_DIR "${CMAKE_SOURCE_DIR}/wfcroot/iow")
set(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
set(WFC_TEST_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}-tests")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

include_directories(wfcroot/wfc)
include_directories(wfcroot/iow)
include_directories(wfcroot/wlog)
include_directories(wfcroot/wjson)
include_directories(wfcroot/wjrpc)
include_directories(wfcroot/wflow)
include_directories(wfcroot/wrtstat)
include_directories(wfcroot/faslib)
