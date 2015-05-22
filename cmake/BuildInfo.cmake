#INCLUDE(ConfigureLibrary)
#INCLUDE(FindThreads)

#if (NOT CMAKE_BUILD_TYPE)
#  message(STATUS "No build type selected, default to Release")
#  set(CMAKE_BUILD_TYPE "Release")
#endif()

#IF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")
#    SET(CMAKE_CXX_STANDARD    "-std=c++11")
#    SET(CMAKE_CXX_WARN_FLAGS  "-W -Wall -pedantic")
#    SET(CMAKE_CXX_DEFINES     "-D_THREAD_SAFE -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -DBOOST_SYSTEM_NO_DEPRECATED ")
#    SET(CMAKE_CXX_ARCH_FLAGS  "-mfpmath=sse -msse2")
#    SET(CMAKE_CXX_FLAGS       "${CXX_FLAGS} ${CMAKE_CXX_STANDARD} ${CMAKE_CXX_WARN_FLAGS} ${CMAKE_CXX_DEFINES} ${CMAKE_CXX_ARCH_FLAGS}")
#    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${CMAKE_CXX_FLAGS}")
#    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -march=core2 ${CMAKE_CXX_FLAGS}")
#    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb -march=core2 ${CMAKE_CXX_FLAGS}")
#    SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb ")
#    SET(CMAKE_C_FLAGS_RELEASE "-O2 -ggdb -march=core2")
#ENDIF("${CMAKE_COMPILER_IS_GNUCXX}" MATCHES "1")


GET_FILENAME_COMPONENT(CURRENT_SCRIPT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)

MACRO(build_info target_name prefix)
  add_custom_target(
    ${prefix}_build_info
    COMMAND
      /bin/bash ${CURRENT_SCRIPT_DIRECTORY}/../build_info.sh ${prefix} ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Create build info files"
    VERBATIM
  )
  add_dependencies(${target_name} ${prefix}_build_info)
  target_link_libraries(${target_name} ${CMAKE_BINARY_DIR}/${prefix}_build_info.a)
  SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/build_info.h")
ENDMACRO(build_info)

#
# Boost filesystem
#
#CONFIGURE_LIBRARY(boost/system/error_code.hpp  "/usr/include /usr/local/include"
#                  boost_system-mt   "/lib /usr/lib /usr/local/lib /usr/lib64")
