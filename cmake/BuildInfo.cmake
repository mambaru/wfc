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

MACRO(build_info target_name display_name)
  set(build_dir ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}Info)
  set(build_prefix ${build_dir}/${display_name}_build_info)
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" cxx_build_type_flags_name)
  set(cxx_build_type_flags "${CMAKE_CXX_FLAGS} ${${cxx_build_type_flags_name}}")

  add_custom_target(
    ${display_name}_build_info
    COMMAND
      /bin/bash ${CURRENT_SCRIPT_DIRECTORY}/../build_info.sh 
        ${display_name}
        ${build_dir}
        ${CMAKE_BUILD_TYPE}
        ${CMAKE_CXX_COMPILER}
        ${cxx_build_type_flags}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )
  add_dependencies(${target_name} ${display_name}_build_info)

  # message(FATAL_ERROR ${build_prefix}.a)
  target_link_libraries(${target_name} ${build_prefix}.a)
  set(clean_list "${CMAKE_CURRENT_SOURCE_DIR}/${display_name}_build_info.h;${build_prefix}1.c;${build_prefix}2.c;${build_prefix}1.c~1;${build_prefix}.a;${build_prefix}1.o;${build_prefix}2.o;")
  SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_list}" )
ENDMACRO(build_info)

#
# Boost filesystem
#
#CONFIGURE_LIBRARY(boost/system/error_code.hpp  "/usr/include /usr/local/include"
#                  boost_system-mt   "/lib /usr/lib /usr/local/lib /usr/lib64")
