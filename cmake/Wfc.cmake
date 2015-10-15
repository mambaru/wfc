message(STATUS " tmp -------------> ${CMAKE_CURRENT_SOURCE_DIR} ready=${WFC_READY} ")

if ( WFC_READY )
  message(STATUS "WFC_READY ON ${CMAKE_CURRENT_SOURCE_DIR}")
  return()
endif()

INCLUDE(ConfigureLibrary)
INCLUDE(FindThreads)

set(WFC_READY ON)

message(STATUS " NEW -------------> ${CMAKE_CURRENT_SOURCE_DIR}")

set(Boost_USE_MULTITHREADED ON)  

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if ( WFC_BUILD_STATIC )
  message(STATUS "WFC build static")
  set(BUILD_SHARED_LIBS OFF)
elseif( WFC_BUILD_STATIC_BOOST)
  message(STATUS "WFC and BOOST build static")
  set(BUILD_SHARED_LIBS OFF)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()
#if ( NOT WFC_BULD_STATIC )
#  message(STATUS "Default build shared libs. Use -DWFC_BULD_STATIC for static build")
#  SET(BUILD_SHARED_LIBS ON)
#  set(Boost_USE_STATIC_LIBS OFF) 
#  set(Boost_USE_STATIC_RUNTIME OFF) 
#else()
#  message(STATUS "Build static libs.")
#  SET(BUILD_SHARED_LIBS OFF)
#  set(Boost_USE_STATIC_LIBS ON) 
#  set(Boost_USE_STATIC_RUNTIME ON) 
#endif()

find_package(Boost COMPONENTS system program_options filesystem REQUIRED)

#if (BUILD_SHARED_LIBS:BOOL!=OFF )
  # -DBUILD_SHARED_LIBS:BOOL=OFF
#  message(STATUS "Default build shared libs ${BUILD_SHARED_LIBS}")
#  SET(BUILD_SHARED_LIBS ON)
#endif()

# -DBUILD_SHARED_LIBS:BOOL=OFF
# SET(BUILD_SHARED_LIBS ON)



IF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")
    exec_program(
      ${CMAKE_CXX_COMPILER}
      ARGS -dumpversion
      OUTPUT_VARIABLE gcc_compiler_version
    )        

    STRING(REGEX REPLACE "^([0-9]+)\\..*" "\\1" gcc_major_version "${gcc_compiler_version}")
    STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" gcc_minor_version "${gcc_compiler_version}")
    MATH(EXPR gcc_version_number "${gcc_major_version} * 1000 + ${gcc_minor_version}" )

    message(STATUS "C++ compiler version: ${gcc_compiler_version} major: ${gcc_major_version} minor: ${gcc_minor_version} number: ${gcc_version_number} [${CMAKE_CXX_COMPILER}]")

    SET(CMAKE_CXX_STANDARD    "-std=c++0x")
    if ( NOT ${gcc_version_number} LESS 4008 )
      SET(CMAKE_CXX_STANDARD    "${CMAKE_CXX_STANDARD} -ftemplate-backtrace-limit=0")
    endif()
    SET(CMAKE_CXX_WARN_FLAGS  "-W -Wall -pedantic -Wextra")
    SET(CMAKE_CXX_DEFINES     "-D_THREAD_SAFE -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DBOOST_SYSTEM_NO_DEPRECATED -D_GLIBCXX_USE_NANOSLEEP")
    SET(CMAKE_CXX_ARCH_FLAGS  "-mfpmath=sse -msse2")
    SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${CMAKE_CXX_STANDARD} ${CMAKE_CXX_WARN_FLAGS} ${CMAKE_CXX_DEFINES} ${CMAKE_CXX_ARCH_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -DNDEBUG -march=core2 ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb ")
    SET(CMAKE_C_FLAGS_RELEASE "-O2 -ggdb -march=core2 -DNDEBUG")
ENDIF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")

SET(FASLIB_DIR "${CMAKE_SOURCE_DIR}/faslib")
SET(IOWLIB_DIR "${CMAKE_SOURCE_DIR}/iow")
SET(FAS_TESTING_CPP "${FASLIB_DIR}/fas/testing/testing.cpp")
##set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(WFC_TEST_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}-tests")

#

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})


