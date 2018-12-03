set(WLOG_ENABLE_CLOG ON)

INCLUDE(FindThreads)
set(Boost_USE_MULTITHREADED ON)

if( NOT BUILD_SHARED_LIBS)
  message(STATUS "Enable build BOOST static")
  #set(Boost_USE_STATIC_LIBS ON)
  #set(Boost_USE_STATIC_RUNTIME ON)
endif()

if ( WFC_ENABLE_STAT )
  add_definitions(-DWFC_ENABLE_STAT)
endif()

find_package(Boost COMPONENTS system program_options filesystem date_time regex REQUIRED)

set(FASLIB_DIR "${CMAKE_SOURCE_DIR}/wfcroot/faslib")
set(IOWLIB_DIR "${CMAKE_SOURCE_DIR}/wfcroot/iow")
set(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
set(WFC_TEST_DIR "${CMAKE_BINARY_DIR}/tests/${CMAKE_CURRENT_PROJECT_NAME}")

include_directories(wfcroot/wfc)
include_directories(wfcroot/iow)
include_directories(wfcroot/wlog)
include_directories(wfcroot/wjson)
include_directories(wfcroot/wjrpc)
include_directories(wfcroot/wflow)
include_directories(wfcroot/wrtstat)
include_directories(wfcroot/faslib)
